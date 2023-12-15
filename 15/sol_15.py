import os

def get_data():
    data_path = os.path.dirname(__file__)
    data_path = os.path.join(data_path, "data.txt")
    file = open(data_path)
    data_in = file.readlines()[0].rstrip().split(',')
    
    return data_in


def hash_string(str_in: str):
    hash_val = 0

    for c in str_in:
        hash_val += ord(c)
        hash_val *= 17
        hash_val %= 256
    
    return hash_val

def process_operation(box_dict: dict, op: str):
    label = ""
    value = ""
    if '-' in op:
        label = op.split('-')[0]
        box_id = hash_string(label)
        for l in box_dict[box_id]:
            if label == l[0]:
                box_dict[box_id].remove(l)
                return
    else:
        label, value = op.split('=')
        value = int(value)
        box_id = hash_string(label)

        for idx,l in enumerate(box_dict[box_id]):
            if label == l[0]:
                box_dict[box_id].remove(l)
                box_dict[box_id].insert(idx,(label,value))
                return

        box_dict[box_id].append((label,value))

def calc_focal_power(box_dict: dict):
    focal_power = 0

    for key, val in box_dict.items():
        f1 = key+1
        for idx,lens in enumerate(val):
            focal_power += f1 * (idx+1) * lens[1]

    return focal_power


if __name__ == '__main__':
    unhashed_data = get_data()

    # Part 1
    sum = 0
    for sub_str in unhashed_data:
        sum += hash_string(sub_str)
    print(f"Part 1: {sum}")

    # Part 2
    box_dict = {x: [] for x in range(0,256)}
    for sub_str in unhashed_data:
        process_operation(box_dict, sub_str)
        
    print(f"Part 2: {calc_focal_power(box_dict)}")
