import datetime

d = datetime.datetime.now()
xapp1232_date = d.second + (d.minute<<6) + (d.hour<<12) + ((d.year-2000)<<17) + (d.month<<23) + (d.day<<27)

ofile = open("timestamp.v", "w")
ofile.write("// dynamically generated at buildtime; timestamp as per Xilinx XAPP1232\n")
ofile.write("assign buildtime = 32'd%d;\n" % xapp1232_date)
ofile.close()

