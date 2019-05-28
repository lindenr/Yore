#!/bin/python
import re

def mons_poll(x, s):
	if s['param_type'] == 'MonsID' and '(nopoll)' not in s['param_options']:
		return '''
			if (mons_is (ev->{0}.{1}))
				mons_poll (ev->{0}.{1});'''.format(x['ev_name'], s['param_name'])
	return ''

def post_string(x):
	return ''.join([mons_poll(x, s) for s in x['ev_params']])

def arg_string(name, s):
	return 'ev->{}.{}'.format(name, s['param_name'])

#def darg_string(name, s):
#	return '{} {}'.format(s['param_type'], s['param_name'])

def and_string(name, s):
	if s['param_type'] == 'MonsID':
		return 'mons_is (ev->{}.{})'.format(name, s['param_name'])
	elif s['param_type'] == 'ItemID':
		return 'it_is (ev->{}.{})'.format(name, s['param_name'])
	print(s[0])
	return ''

def myjoin(st, f, x):
	return st.join ([f (x['ev_name'], s) for s in x['ev_params']])

def if_fun(x):
	z = ' && '.join ([and_string(x['ev_name'], s) for s in x['ev_params'] if
		s['param_type'] in {'MonsID', 'ItemID'} and
		'(optional)' not in s['param_options']
		])
	#, dict(ev_name = x['ev_name'], ev_params = [[s[0]] for s in x['ev_params'] if s[0].split()[0] in {'MonsID', 'ItemID'} and '(optional)' not in s[1]]))
	if z == '':
		return ''
	return 'if ('+z+')\n\t\t\t\t'

def mons_status(x):
	if len(x['ev_params']) == 0 or x['ev_status'] == [] or x['ev_status'][0] != 'mons' or not x['ev_status_first']:
		return ''
	return """
struct
{{
	EvID evID;
	Tick due;{}
}} {};""".format(''.join(['\n\t' + s['param_type'] + ' ' + s['param_name'] + ';' for s in x['ev_params'][1:]]), x['ev_status'][1])

def item_status(x):
	if len(x['ev_params']) == 0 or x['ev_status'] == [] or x['ev_status'][0] != 'item' or not x['ev_status_first']:
		return ''
	return """
struct
{{
	EvID evID;
	Tick due;{}
}} {};""".format(''.join(['\n\t' + s['param_type'] + ' ' + s['param_name'] + ';' for s in x['ev_params'][1:]]), x['ev_status'][1])

def check_state(x):
	if x['ev_status'] == []:
		return ''
	if x['ev_status'][0] == 'mons':
		return '''
			if (qev->ID != mons_internal(ev->{0}.{1})->status.{2}.evID)
				return;'''.format (x['ev_name'], x['ev_params'][0]['param_name'], x['ev_status'][1])
	elif x['ev_status'][0] == 'item':
		return '''
			if (qev->ID != it_internal(ev->{0}.{1})->status.{2}.evID)
				return;'''.format (x['ev_name'], x['ev_params'][0]['param_name'], x['ev_status'][1])
	assert (0)

def finish_state(x):
	if not x['ev_status_last']:
		return ''
	if x['ev_status'][0] == 'mons':
		return '''
			if (mons_is (ev->{0}.{1}))
				mons_internal (ev->{0}.{1})->status.{2}.evID = 0;'''.format (
					x['ev_name'], x['ev_params'][0]['param_name'], x['ev_status'][1])
	if x['ev_status'][0] == 'item':
		return '''
			if (it_is (ev->{0}.{1}))
				it_internal (ev->{0}.{1})->status.{2}.evID = 0;'''.format (
					x['ev_name'], x['ev_params'][0]['param_name'], x['ev_status'][1])
	assert (0)

def case(x):
	return """
		case EV_{0}:
		{{{4}{5}
			{3}ev_{0} ({1});{2}
			return;
		}}""".format (x['ev_name'], myjoin (', ', arg_string, x), post_string (x), if_fun (x), check_state(x), finish_state(x))

def event_qdecl(x):
	return """void ev_queue_{0} (Tick udelay{1});
""".format(x['ev_name'], ''.join([', ' + s['param_decl'] for s in x['ev_params']]))

def event_queue(x):
	status_str = ''
	var_str = ''
	if x['ev_status'] != [] and x['ev_status_first']:
		var_str = 'struct QEv *qev = '
		if x['ev_status'][0] == 'mons':
			status_str = '''
	mons_internal ({0})->status.{1} = (typeof(mons_internal ({0})->status.{1})) {{qev->ID, qev->tick{2}}};'''.format(x['ev_params'][0]['param_name'], x['ev_status'][1], ''.join([', ' + s['param_name'] for s in x['ev_params'][1:]]), x['ev_name'])
		if x['ev_status'][0] == 'item':
			status_str = '''
	it_internal ({0})->status.{1} = (typeof(it_internal ({0})->status.{1})) {{qev->ID, qev->tick{2}}};'''.format(x['ev_params'][0]['param_name'], x['ev_status'][1], ''.join([', ' + s['param_name'] for s in x['ev_params'][1:]]), x['ev_name'])
	return """
void ev_queue_{0} (Tick udelay{3})
{{
	{2}ev_queue_aux (udelay, (union Event) {{ .{0} = {{EV_{0}{4}}}}});{1}
}}
""".format(x['ev_name'], status_str, var_str, ''.join([', ' + s['param_decl'] for s in x['ev_params']]), ''.join([', ' + s['param_name'] for s in x['ev_params']]))

def main():
	in_file = open('gen/event.h.gen')
	parsed = []
	for line in in_file:
		line = re.sub (r'//.*', '', line).strip()
		if len(line) == 0:
			continue
		if line[0] == '|':
			a = line.split()
			if len(a) <= 2:
				parsed.append (dict(
					ev_name = a[1],
					ev_params = [],
					ev_status = [],
					ev_status_first = False,
					ev_status_last = False))
			else:
				parsed.append (dict(
					ev_name = a[1],
					ev_params = [],
					ev_status = a[2].split('.'),
					ev_status_first = True,
					ev_status_last = True))
			continue
		elif line[0] == '>':
			parsed[-1]['ev_status_last'] = False
			a = line.split()
			parsed.append (dict(
				ev_name = a[1],
				ev_params = [],
				ev_status = parsed[-1]['ev_status'],
				ev_status_first = False,
				ev_status_last = True))
			continue
		elif len(parsed) == 0:
			print("Entry must start with '|'")
			return
		options = set()
		segs = line.split()
		for seg in segs:
			if seg[0] == '(':
				options.add (seg)
		segs = [seg for seg in segs if seg[0] != '(']
		parsed[-1]['ev_params'].append (dict(
			param_type = ' '.join(segs[:-1]),
			param_name = segs[-1],
			param_decl = ' '.join(segs),
			param_options = options))
	out_1 = open('auto/event.enum.h', 'w')
	out_1.write ('//generated!\n' + ',\n'.join(['\tEV_'+x['ev_name'] for x in parsed]))
	out_2 = open('auto/event.union.h', 'w')
	out_2.write ('//generated!\n' + ''.join([
"""	struct
	{
		EV_TYPE type;
""" + "".join(['\t\t' + s['param_decl'] + ';\n' for s in x['ev_params']]) + "\t} " + x['ev_name'] + ";\n" for x in parsed]))
	out_3 = open('auto/event.switch.h', 'w')
	switch = ''.join ([case(x) for x in parsed])
	out_3.write(
"""//generated!

void ev_do (const struct QEv *qev)
{{
	const union Event *ev = &qev->ev;
	switch (ev->type)
	{{{}
	}}
}}

""".format(switch))
	#print ('\n'.join(['void ev_{} ({})'.format(x['ev_name'], myjoin (', ', darg_string, x)) for x in parsed]))
	out_4 = open('auto/monst.status.h', 'w')
	out_4.write('//generated!\n' + ''.join([mons_status(x) for x in parsed]))
	out_5 = open('auto/event.qdecl.h', 'w')
	out_5.write('//generated!\n' + ''.join([event_qdecl(x) for x in parsed]))
	out_6 = open('auto/event.queue.h', 'w')
	out_6.write('''//generated!
''' + ''.join([event_queue(x) for x in parsed]))
	out_7 = open('auto/item.status.h', 'w')
	out_7.write('''//generated!
''' + ''.join([item_status(x) for x in parsed]))

if __name__ == "__main__":
	main()
