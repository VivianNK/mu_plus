# Collects file from UEFI Event Audit app, parses it, and generates a HTML report
#
# Copyright (C) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import logging
import operator
import glob
import json
import datetime
import os
import sys
import argparse

#Add script dir to path for import
sp = os.path.dirname(os.path.realpath(sys.argv[0]))
sys.path.append(sp)

from EventInfoObject import *


VERSION = "0.90"

class ParsingTool(object):
    def __init__(self, DatFolderPath):
        self.Logger = logging.getLogger("ParsingTool")
        self.MemoryAttributesTable = []
        self.MemoryRangeInfo = []
        self.PageDirectoryInfo = []
        self.DatFolderPath = DatFolderPath
        self.ErrorMsg = []
        self.PlatformName = PlatformName
        self.PlatformVersion = PlatformVersion
        self.Type = Type
        self.AddressBits = 0


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
