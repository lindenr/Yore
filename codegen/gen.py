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
	if z == '':
		return ''
	return 'if ('+z+')\n\t\t\t\t'

def st_status(x, st):
	if (len(x['ev_params']) == 0 or
		x['ev_status'] == [] or
		x['ev_status'][0] != st or
		not x['ev_status_first']):
		return ''
	return """
struct
{{
	EvID evID;
	Tick due;{}
}} {};""".format(
	''.join(['\n\t{} {};'.format (s['param_type'], s['param_name']) for s in x['ev_params'][1:]]),
	x['ev_status'][1])

def ev_status(x):
	return '{3}_internal(ev->{0}.{1})->status.{2}'.format (
					x['ev_name'],
					x['ev_params'][0]['param_name'],
					x['ev_status'][1],
					x['ev_status'][0])

def ev_is(x):
	return '{}_is (ev->{}.{})'.format (
		x['ev_status'][0],
		x['ev_name'],
		x['ev_params'][0]['param_name'])

def check_state(x):
	if x['ev_status'] == []:
		return ''
	return '''
			if ((!{}) ||
				qev->ID != {}.evID)
				return;'''.format (ev_is (x), ev_status (x))

def finish_state(x):
	if not x['ev_status_last']:
		return ''
	return '''
			if ({})
				{}.evID = 0;'''.format (ev_is (x), ev_status (x))

def case(x):
	return """
		case EV_{0}:
		{{{4}{5}
			{3}ev_{0} ({1});{2}
			return;
		}}""".format (
			x['ev_name'],
			myjoin (', ', arg_string, x),
			post_string (x),
			if_fun (x),
			check_state(x),
			finish_state(x))

def event_qdecl(x):
	return """void ev_queue_{0} (Tick udelay{1});
""".format(x['ev_name'], ''.join([', ' + s['param_decl'] for s in x['ev_params']]))

def event_queue(x):
	if x['ev_status'] == []:
		status_str = ''
		var_str = ''
	else:
		var_str = 'struct QEv *qev = '
		if x['ev_status_first']:
			status_str = '''
	{3}_internal ({0})->status.{1} = (typeof({3}_internal ({0})->status.{1})) {{qev->ID, qev->tick{2}}};'''
		else:	
			status_str = '''
	{3}_internal ({0})->status.{1}.evID = qev->ID;'''
		status_str = status_str.format(
			x['ev_params'][0]['param_name'],
			x['ev_status'][1],
			''.join([', ' + s['param_name'] for s in x['ev_params'][1:]]),
			x['ev_status'][0])
	return """
void ev_queue_{0} (Tick udelay{3})
{{
	{2}ev_queue_aux (udelay, (union Event) {{ .{0} = {{EV_{0}{4}}}}});{1}
}}
""".format(
	x['ev_name'],
	status_str,
	var_str,
	''.join([', ' + s['param_decl'] for s in x['ev_params']]),
	''.join([', ' + s['param_name'] for s in x['ev_params']]))

def ev_mons_can(x):
	return '''
case EV_{0}:
{{
	return 1;
}}'''.format(x['ev_name'])

def my_fwrite(name, cont):
	open('auto/'+name, 'w').write('''// Generated code - don't change directly or it will be overwritten.

#ifndef {0}
#define {0}
{1}

#endif /* {0} */

'''.format (name.replace('.', '_'), cont))

def main():
	in_file = open('gen/events.gen')
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
					ev_status_last = False,
					ev_options = set()))
			else:
				st = a[2].split('.')
				if st[0] == 'item':
					st[0] = 'it'
				parsed.append (dict(
					ev_name = a[1],
					ev_params = [],
					ev_status = st,
					ev_status_first = True,
					ev_status_last = True,
					ev_options = set(a[3:])))
			continue
		elif line[0] == '>':
			parsed[-1]['ev_status_last'] = False
			a = line.split()
			parsed.append (dict(
				ev_name = a[1],
				ev_params = [],
				ev_status = parsed[-1]['ev_status'],
				ev_status_first = False,
				ev_status_last = True,
				ev_options = parsed[-1]['ev_options']))
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
	my_fwrite('event.enum.h',
'''enum Ev_type
{{
{}
}};'''.format (',\n'.join(['\tEV_'+x['ev_name'] for x in parsed])))
	my_fwrite ('event.union.h', ''.join([
"""	struct
	{
		Ev_type type;
""" + "".join(['\t\t' + s['param_decl'] + ';\n' for s in x['ev_params']]) + "\t} " + x['ev_name'] + ";\n" for x in parsed]))
	switch = ''.join ([case(x) for x in parsed])
	my_fwrite ('event.switch.h', """
void ev_do (const struct QEv *qev)
{{
	const union Event *ev = &qev->ev;
	switch (ev->type)
	{{{}
	}}
}}""".format(switch))
	#print ('\n'.join(['void ev_{} ({})'.format(x['ev_name'], myjoin (', ', darg_string, x)) for x in parsed]))
	my_fwrite ('monst.status.h', ''.join([st_status(x, 'mons') for x in parsed]))
	my_fwrite ('event.qdecl.h', ''.join([event_qdecl(x) for x in parsed]))
	my_fwrite ('event.queue.h', ''.join([event_queue(x) for x in parsed]))
	my_fwrite ('item.status.h', ''.join([st_status(x, 'it') for x in parsed]))
	my_fwrite ('event.mons_can.h', ''.join([ev_mons_can(x) for x in parsed]))
	#out_8.write('''//generated

if __name__ == "__main__":
	main()

