import math
import os

def get_data():
    data_path = os.path.dirname(__file__)
    data_path = os.path.join(data_path, "data.txt")
    file = open(data_path)
    dir_commands = ""
    node_map = {}

    for line in file:
        line = line.rstrip()
        if not line:
            continue
        if dir_commands == "":
            dir_commands = line
        else:
            node_name_split,_, left,right = line.split()
            neigh_list = [left[1:-1], right[:-1]]
            node_map[node_name_split] = neigh_list
    
    return dir_commands, node_map

def sol_part_1(dir_commands, node_map):
    cur_node = 'AAA'
    target_node = 'ZZZ'
    step_count = 0

    while (cur_node != target_node):
        # get current left/right direction
        cur_cmd = dir_commands[step_count % len(dir_commands)]

        # get next node
        if cur_cmd == "L":
            cur_node = node_map[cur_node][0]
        else:
            cur_node = node_map[cur_node][1]

        step_count += 1

    return step_count

def get_start_nodes(node_map):
    start_nodes =[]

    for key, val in node_map.items():
        if key[-1] == 'A':
            start_nodes.append(key)
    
    return start_nodes    

def all_end_with_z(items):
     return all('Z' == x[-1] for x in items)

def get_lcm(cycle_times):
    return math.lcm(*cycle_times)

def sol_part_2(dir_commands, node_map):
    cur_nodes = get_start_nodes(node_map)
    step_count = 0

    debug_dict = {}

    while len(debug_dict) != len(cur_nodes):
        # get current left/right direction
        cur_cmd = dir_commands[step_count % len(dir_commands)]

        step_count += 1
        # get next node for each cur_node
        for i in range(len(cur_nodes)):
            if cur_cmd == "L":
                cur_nodes[i] = node_map[cur_nodes[i]][0]
            else:
                cur_nodes[i] = node_map[cur_nodes[i]][1]
            if cur_nodes[i][-1] == 'Z' and not i in debug_dict.keys() :
                debug_dict[i] = step_count

    return get_lcm(list(debug_dict.values()))

if __name__ == '__main__':
    dir_commands, node_map = get_data()

    print(sol_part_1(dir_commands, node_map))
    print(sol_part_2(dir_commands, node_map))

