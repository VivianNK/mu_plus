import copy
import logging

# Event Info object used in Event Audit App
#
# Copyright (C) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
class EventInfo(object):
    image_path = ""
    function_addr = ""
    time_ns = 0
    tpl = 0
    function_name = ""

    # 
    def __init__(self, image_path, function_addr, time_ns, tpl):
        self.image_path = image_path
        self.function_addr = function_addr
        self.time_ns = time_ns
        self.tpl = tpl
