## 
# EventInfoTmp.py
# script to parse through build files for function name and more data
# # 
from tabulate import tabulate

# path to event debug log
eventlog_file_path = "C:\\msft\\test1\\event_debug_new.txt"
# eventlog_file_path = "C:\\msft\\test1\\EventAudit.dat"


# path to out file
out_path = "C:\\msft\\test1\\event_timeline.txt"


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

    def __str__(self):
        return ""
    
    def fields_as_str_arr(self):
        return ["{0}".format(self.image_path), "{0}".format(self.function_name), "{0}".format(self.time_ns), "{0}".format(self.tpl)]

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


def parse_event_debug(lines_iter, events_called):
    start_time = None
    # todo subtract start time from all
    try:
        for line in lines_iter:
            tokens = line.split(",")
            event = EventInfo(tokens[0], tokens[1], tokens[2], tokens[3])
            map_path = get_image_map_path(tokens[0]) 
               
            # todo remove "0x" from function addr?
            event.function_name = get_func_name(map_path, event.function_addr[2:])

            if event.function_name is None:
                print("DEBUG INFO: error - address " + event.function_addr[2:] + " not found in map file")
            else:
                out_str = event.function_name + "\t" + event.time_ns + event.tpl + event.image_path + "\n"
                event.image_path = event.image_path[63:]
                events_called.append(event.fields_as_str_arr())
                    
    except StopIteration as ex:
        print("Exception thrown: Stop Iteration - reached EOF")

    finally:
        print("Exiting from parse_event_debug")


if __name__ == '__main__':
    log_file = open(eventlog_file_path, 'r')
    lines = log_file.readlines()
    lines_iter = iter(lines)
    
    events_called = []
    events_called.append(["Image Path", "Function Name", "Timestamp", "Tpl"])

    parse_event_debug(lines_iter, events_called)    
    
    ##newf = open(out_path, "w")
    ##newf.writelines(events_called)
    ##newf.close()
    print("Printed " + str(len(events_called)-1) + " lines to file: " + out_path)

    print(tabulate(events_called, headers='firstrow', tablefmt='fancy_grid'))



