import pickle

data = pickle.load(open('s6lx25-glitchwidth.p'))

#print len(data['base'])

for t in data['values']:
    for j in data['values'][t]:
        print "%04x "%j[0],
    print ""
