#!/bin/python
import re

def prep_string(x, s):
	ret = ''
	return ret
	if (s[0] == 'MonsID'):
		ret = '''
			struct Monster *{0} = mons_at (ev->{1}.{2});'''.format(s[1][:-2], x, s[1])
	elif (s[0] == 'ItemID'):
		ret = '''
			struct Item *{0} = it_at (ev->{1}.{2});'''.format(s[1][:-2], x, s[1])
	return ret

def post_string(x, s):
	if s[0] == 'MonsID':
		return '''
			if (mons_is (ev->{0}.{1}))
				mons_poll (ev->{0}.{1});'''.format(x,s[1])
	return ''

def arg_string(x, s):
	#if s[0] == 'MonsID' or s[0] == 'ItemID':
	#	return s[1][:-2]
	return 'ev->{}.{}'.format(x, s[1])

def darg_string(x, s):
	#if s[0] == 'MonsID':
	#	return 'struct Monster *'+s[1][:-2]
	#elif s[0] == 'ItemID':
	#	return 'struct Item *'+s[1][:-2]
	return '{} {}'.format(s[0], s[1])

def and_string(x, s):
	if s[0] == 'MonsID':
		return 'mons_is (ev->{}.{})'.format(x,s[1])
	elif s[0] == 'ItemID':
		return 'it_is (ev->{}.{})'.format(x,s[1])
	return ''

def myjoin(a, f, x):
	return a.join ([f (x[0], (lambda a: [' '.join(a[:-1]), a[-1]]) (s[:-2].split())) for s in x[1]])

def if_fun(z):
	if z == '':
		return ''
	return 'if ('+z+')\n\t\t\t\t'

def mons_status(x):
	if len(x[1]) == 0 or x[1][0].split()[0] != 'MonsID' or x[2] == '' or x[3] == 'inherit':
		return ''
	return """
struct
{{
	EvID evID;
	Tick due;{}
}} {};""".format(''.join([(lambda a: '\n\t' + ' '.join(a[:-1]) + ' ' + a[-1] + ';') (s[:-2].split()) for s in x[1][1:]]), x[2])

def check_state(x):
	if x[2] == '':
		return ''
	return '''
			if (qev->ID != mons_internal(ev->{}.{})->status.{}.evID)
				return;'''.format (x[0], x[1][0][:-2].split()[1], x[2])

def finish_state(x):
	if x[4] != 'f':
		return ''
	return '''
			if (mons_is (ev->{0}.{1}))
				mons_internal (ev->{0}.{1})->status.{2}.evID = 0;'''.format (x[0], x[1][0][:-2].split()[1], x[2])

def case(x):
	return """
		case EV_{0}:
		{{{5}{1}
			{4}ev_{0} ({2});{6}{3}
			return;
		}}""".format (x[0], myjoin ('', prep_string, x), myjoin (', ', arg_string, x), myjoin('', post_string, x), if_fun (myjoin(' && ', and_string, [x[0],[a for a in x[1] if a.split()[0] in {'MonsID', 'ItemID'}]])), check_state(x), finish_state(x))

def event_queue(x):
	return """void ev_queue_{0} (Tick udelay, union Event ev);
""".format(x[0])

def event_queue_c(x):
	status_str = ''
	var_str = ''
	if x[2] != '' and x[3] == 'og':
		var_str = 'struct QEv *qev = '
		status_str = '''
	mons_internal (ev.{3}.{0})->status.{1} = (typeof(mons_internal (ev.{3}.{0})->status.{1})) {{qev->ID, qev->tick{2}}};'''.format(x[1][0][:-2].split()[1], x[2], ''.join([', ev.'+x[0]+'.' + s[:-2].split()[-1] for s in x[1][1:]]), x[0])
	return """
void ev_queue_{0} (Tick udelay, union Event ev)
{{
	{2}ev_queue_aux (udelay, ev);{1}
}}
""".format(x[0], status_str, var_str)

def main():
	in_file = open('gen/event.h.gen')
	parsed = []
	for line in in_file:
		line = re.sub (r'//.*', '', line)
		if len(line) < 2:
			continue
		if line[0] == '|':
			a = line.split()
			if len(a) <= 2:
				parsed.append ([a[1], [], '', '', ''])
			else:
				parsed.append ([a[1], [], a[2][1:], 'og', 'f'])
			continue
		elif line[0] == '>':
			parsed[-1][4] = ''
			a = line.split()
			parsed.append ([a[1], [], parsed[-1][2], 'inherit', 'f'])
			#a = ''.join(line.split()[1:]).split('->')
			#parsed.append ([a, []])
			continue
		elif len(parsed) == 0:
			print("Entry must start with '|'")
			return
		parsed[-1][1].append ("\t" + line[:-1]+";\n")
	out_1 = open('auto/event.enum.h', 'w')
	out_1.write ('//generated!\n' + ',\n'.join(['\tEV_'+x[0] for x in parsed]))
	out_2 = open('auto/event.union.h', 'w')
	out_2.write ('//generated!\n' + ''.join([
"""	struct
	{
		EV_TYPE type;
""" + "".join(x[1]) + "\t} " + x[0] + ";\n" for x in parsed]))
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
	#print ('\n'.join(['void ev_{} ({})'.format(x[0], myjoin (', ', darg_string, x)) for x in parsed]))
	out_4 = open('auto/monst.status.h', 'w')
	out_4.write('//generated!\n' + ''.join([mons_status(x) for x in parsed]))
	out_5 = open('auto/event.qdecl.h', 'w')
	out_5.write('//generated!\n' + ''.join([event_queue(x) for x in parsed]))
	out_6 = open('auto/event.queue.h', 'w')
	out_6.write('''//generated!
''' + ''.join([event_queue_c(x) for x in parsed]))

if __name__ == "__main__":
	main()
