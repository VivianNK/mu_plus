# Collects file from UEFI Event Audit app, parses it, and generates a HTML report
#
# Copyright (C) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import logging
import operator
import glob
import json
import csv
import datetime
import os
from re import T
import sys
import argparse
# input csv path = "C:\\msft\\EventAudit.csv"
# output html path = "C:\\msft\\EventReport.html"

#Add script dir to path for import
sp = os.path.dirname(os.path.realpath(sys.argv[0]))
sys.path.append(sp)

reportGenVersion = '1.01'

from EventInfoObject import *

def get_func_name(map_path, img_offset_hex):
    #
    # get function name from map file using offset
    #
    map_file = open(map_path, 'r')
    mapfile_lines = map_file.readlines()
    map_fname = mapfile_lines[0]
    
    offset_len = len(img_offset_hex)

    # TODO use regex? this seems ok, get 4th token, make sure it's the right length
    # search address lines for matching hex number
    func_name = None          
    debug_ctr = 0
    for addr_line in mapfile_lines:
        addr_line = addr_line.strip()
        tokens = addr_line.split()

        if len(tokens) < 4:
            continue
        
        addr_full = tokens[2]
        if len(addr_full) != 16:
            continue

        debug_ctr += 1
        addr = addr_full[-offset_len:]

        if addr.lower() == img_offset_hex.lower():
            func_name = tokens[1]
    
    map_file.close()
    return func_name


def get_image_map_path(path):
    return path[0:-3] + "map" # repl .pdb with .map


def main():
     # Set up command line arguments
    parser = argparse.ArgumentParser(description='Event Audit Parser Tool')
    #parser.add_argument('-t', '--OutputText', dest='output_text_file', help='Name of the output text file which will contain the boot timing info', default=None)
    parser.add_argument('-i', '--InputCsvFile', help='Path to the input Csv file with raw data', default=None)
    parser.add_argument('-r', '--OutputHtml', dest='output_html_file', help='Name of the output HTML file which will contain the boot timing info', default=None)
    #parser.add_argument('-s', '--SrcTreeRoot', help="Root of UEFI Code tree to parse for guids", nargs="*", default=None)
    #parser.add_argument('-d', '--debug', help='enable debug output', action='store_true')
    #parser.add_argument("-l", "--OutputDebugLog", dest='OutputDebugLog', help="Log all debug and error output to file", default=None)
    options = parser.parse_args()

    html_report = None
    if (options.output_html_file):
        if(len(options.output_html_file) < 2):
            logging.critical('The output html file parameter is invalid')
            return -1
        else:
            # Create a new html report file
            html_report = open(options.output_html_file, 'w')

    # if(options.SrcTreeRoot is not None):
    #     for srcTree in options.SrcTreeRoot:
    #         logging.critical("Parsing Code Tree for function names: %s" % srcTree)
    #         # TODO parse for func address

    logging.critical("Parsing Event info csv")

    # list of tuples, where each tuple is (<ID#>, <Event Group GUID>, <GuidValue>, <Img Path>, <Function Name>, <StartTime>, <Duration>, <Tpl>)
    # ID is a counter that starts at 1
    eventList = []
    idCounter = 1

    # dictionary label:startTime
    eventsDictCount = {}

    if options.InputCsvFile is None or not os.path.exists(options.InputCsvFile):
        logging.critical("No Input Csv File")
        return -4

    # parse csv file
    with open(options.InputCsvFile) as csvfile:
        reader = csv.reader((line.replace('\0', '') for line in csvfile), delimiter=',')
        # entry format: Event,[1]<Img Path>,[2]<Function Address Offset>,[3]<StartTime>,[4]<Duration>,[5]<Event Group GUID>,[6]<Tpl>
        for row in reader:
            if len(row) < 6:
                continue
            image_path = row[1]
            function_addr = row[2]
            start_time = float(row[3]) / 1000000 # convert nanoseconds to milliseconds
            duration = float(row[4]) / 1000000
            event_group_guid = row[5]
            tpl = float(row[6])
            build_start = image_path.find('\\Build\\')
            image_path_short = image_path[build_start+7:]

            # get function name
            map_path = get_image_map_path(image_path) 
            function_name = get_func_name(map_path, function_addr[2:])

            if function_name is None:
                logging.error('Error! address %s not found in map file' % function_addr)
                # print("DEBUG INFO: error - address " + function_addr[2:] + " not found in map file")
                function_name = "not found"

            label = image_path_short + '_' + event_group_guid + '_' + function_name 
            # place this into the starts dictionary, but check if one with same label is already there
            if (label in eventsDictCount):
                eventsDictCount[label] = eventsDictCount[label] + 1
            else:
                eventsDictCount[label] = 1
            
            eventList.append((idCounter, event_group_guid, "TBD", image_path_short, function_name, start_time, duration, tpl))
            # out_str = function_name + "\t" + event.time_ns + event.tpl + event.image_path + "\n"
            # events_called.append(event.fields_as_str_arr())
            idCounter += 1

    if (html_report is not None):
        logging.critical("Writing HTML Report")
        template = open(os.path.join(sp, "Event_Report_template.html"), "r")
        # 
        # write every line as it is until super special line to insert json
        # todo - just replace? the one line? build json in string then just write once
        #
        for line in template.readlines():
            # write data as json
            if "%TO_BE_FILLED_IN_BY_PYTHON_SCRIPT%" in line:
                html_report.write("        var JsonData = {\"Data\": {")

                html_report.write("\"Model\": \"%s\"," % "TODO - get model of system?")
                html_report.write("\"UefiVersion\": \"%s\"," % "TODO - get Uefi version?")
                html_report.write("\"DateCollected\": \"%s\"," % datetime.datetime.strftime(datetime.datetime.now(), "%A, %B %d, %Y %I:%M%p" ))
                html_report.write("\"ReportGenVersion\": \"%s\"," % reportGenVersion)
                html_report.write("\"EventData\": [")

                first = True
                for event in eventList:
                    if (not first):
                        html_report.write(",")
                    first = False
                    # event_group_guid, "TBD", image_path_short, function_name, start_time, duration, tpl
                    html_report.write("{\"ID\": \"%s\",\"EventGroupGuid\": \"%s\",\"GuidValue\": \"%s\",\"ImagePath\": \"%s\",\"FunctionName\": \"%s\",\"StartTime\": \"%.6f\",\"Duration\": \"%.6f\",\"Tpl\": \"%.6f\"}" % (event[0], event[1], event[2], event[3], event[4], event[5], event[6], event[7]))
                html_report.write("],")

                html_report.write("}};")
            else:
                html_report.write(line)
        template.close()
        html_report.close()
        return 0

if __name__ == '__main__':
    # todo understand logger and formatter? as i understand i need
    # parse csv to object, serialize object to json, pass to (modified) report template

    #setup main console as logger
    logger = logging.getLogger('')
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter("%(levelname)s - %(message)s")
    console = logging.StreamHandler()
    console.setLevel(logging.CRITICAL)
    console.setFormatter(formatter)
    logger.addHandler(console)

    #call main worker function
    retcode = main()

    if retcode != 0:
        logging.critical("Failed.  Return Code: %i" % retcode)
    #end logging
    logging.shutdown()
    sys.exit(retcode)
