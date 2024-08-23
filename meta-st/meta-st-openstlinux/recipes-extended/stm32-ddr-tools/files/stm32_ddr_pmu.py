#!/usr/bin/python3

import getopt
import re
import sys
import os

def found_perf_with_address(address):
        prefix = "/proc/device-tree"
        try:
            for directoryfolder in os.listdir(prefix):
                if directoryfolder.find("soc") > -1:
                    if os.path.exists(prefix + '/' + directoryfolder + '/' + address):
                        return (prefix + '/' + directoryfolder + '/' + address)
        except OSError:
            pass
        except Exception as exc:
            pass
        return None


with open('/proc/device-tree/compatible', 'r') as compatible:
    compatible_string = compatible.read()
    if 'stm32mp15' in compatible_string:
        word_length  = 4    # Bytes
        burst_length = 8    # Words
        ddr_freq = 533
        ddr_type = 'DDR'
        divider = 1
        clock_name = 'pll2_r'
    elif 'stm32mp13' in compatible_string:
        word_length  = 2    # Bytes
        burst_length = 8    # Words
        ddr_freq = 533
        ddr_type = 'DDR'
        divider = 1
        clock_name = 'pll2_r'
    elif 'stm32mp25' in compatible_string:
        word_length  = 4    # Bytes
        burst_length = 8    # Words
        ddr_freq = 1200
        ddr_type = 'DDR'
        divider = 8
        clock_name = 'ck_icn_ddr'
        try:
            devicetree_path = found_perf_with_address("perf@48041000")
            if devicetree_path is None:
                print("ERROR: Cannot find perf entry on /proc/device-tree/soc* (%s)" % "perf@48041000")
                sys.exit(1)
            with open("%s/st,dram-type" % devicetree_path, 'rb') as tmp:
                t = int.from_bytes(list(tmp.read(4)), byteorder='big', signed=False)
                if t == 0:
                    ddr_type = 'LPDDR4'
                    burst_length = 16
                elif t == 1:
                    ddr_type = 'LPDDR3'
                elif t == 2:
                    ddr_type = 'DDR4'
                elif t == 3:
                    ddr_type = 'DDR3'
        except:
            print("Warning: cannot detect DDR type from devicetree; use defaults")
    else:
        print("ERROR: Cannot detect SoC from devicetree")
        sys.exit(1)

def usage():
    print("Usage:")
    print("  python stm32_ddr_pmu.py [-d <ddr_freq>] -f <perf_file>")
    print("    -d ddr_freq: DDR frequency in MHz (%s MHz by default)" % ddr_freq)
    print("    -f perf_file: text file containing the output of")
    print("    -w word_length: width in bytes of DDR bus (%s by default)" % word_length)
    print("    -l burst_length: length in cycles of DDR burst (%s by default)" % burst_length)
    print("        perf stat -e stm32_ddr_pmu/read_cnt/,stm32_ddr_pmu/time_cnt/,stm32_ddr_pmu/write_cnt/ -a -o <perf_file> <command>")
    print("The script considers bursts of %s words with %s bytes per word." % (burst_length, word_length))
    sys.exit(2)

perf_file = None
dic = {}

with open('/sys/kernel/debug/clk/' + clock_name + '/clk_rate', 'r') as clk_rate:
    clk_rate_Hz = int(clk_rate.readline().strip())
    clk_rate_MHz = clk_rate_Hz / 1000000
if (clk_rate_MHz):
    print("Found ddr frequency of %s MHz" % clk_rate_MHz)
    ddr_freq = clk_rate_MHz
else :
    print('Warning: cannot find ddr clock summary entry, fallback to default value else specified')

try:
    opts, args = getopt.getopt(sys.argv[1:], "d:f:")
except getopt.GetoptError:
    print("Error: invalid option !")
    usage()

for opt,arg in opts:
    if opt == '-d':
        ddr_freq = int(arg)
    elif opt == '-f':
        perf_file = arg
    elif opt == '-w':
        word_length = arg
    elif opt == '-l':
        burst_length = arg
    else:
        usage()

if perf_file == None:
    print("Error: no perf file !")
    usage()

with open(perf_file) as file:
    lines = file.readlines()
    for line in lines:
        a = re.match(".* ([0-9]+).*stm32_ddr_pmu\/(.*)\/.*", line)
        try:
            dic[a.groups()[1]] = a.groups()[0]
        except:
            continue

constant = word_length * burst_length * ddr_freq * 1000000 / int(dic['time_cnt']) / (1024 * 1024) / divider
read_bw = int(dic['read_cnt']) * constant
write_bw = int(dic['write_cnt']) * constant

print("R = %s MB/s, W = %s MB/s, R&W = %s MB/s (%s @ %s MHz)" % (read_bw.__round__(),
      write_bw.__round__(), (read_bw + write_bw).__round__(), ddr_type, ddr_freq))
