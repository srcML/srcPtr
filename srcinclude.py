from __future__ import print_function

import sys
import re
import argparse
import os
import os.path
import subprocess

def main(argv):
    
    #Process Arguments
    parser = argparse.ArgumentParser(description='srcInclude - Program for include analysis and generating srcML based on include order')

    parser.add_argument('-o', nargs=1, type=str, help='file to output srcml to') 
    parser.add_argument('files', metavar='files', type=str, nargs='+', help='files for processing')
    #parser.add_argument('--srcml', action='store', nargs=1, required=False, help='generate srcML based on file include order')

    args = parser.parse_args()



    #Build dictionary of filename -> files it includes
    includes = dict()

    try:
        regex = re.compile('#include\s*["<](.*\.[hc].*)[">]')

        for f in args.files:
            fn = open(f, "r")
            includes[f] = set()

            for line in fn:
                m = regex.search(line)
                
                if m:
                    includes[f].add(m.group(1))

            fn.close()
    except IOError:
        print ("Error: file does not exist")
    

    #Create makefile based on dependencies
    f = open('srcIncludeMakefile', 'w')

    for key, value in includes.items():
        if(os.path.isfile(key)):
            f.write(key + "_srcinclude: ")

            for x in value:
                if(os.path.isfile(x)):
                    f.write(x + "_srcinclude ")

            f.write("\n\t @echo " + key + "\n")

    f.write("all: ")
    for key, value in includes.items():
        if(os.path.isfile(key)):
            f.write(key + "_srcinclude ")

    f.close()

    #Execute srcML based on ouput of makefile
    if not args.o:
       os.system('srcml --in-order $(make all -f srcIncludeMakefile) > out.xml') 
    else:
        os.system('srcml --in-order $(make all -f srcIncludeMakefile) > ' + args.o[0])

if __name__ == "__main__":
    main(sys.argv)