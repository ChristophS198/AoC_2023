#include <string>
#include <set>
#include <unordered_map>
#include <numeric>

#include "../utility.h"


namespace Day19
{
    struct Range;
    
    using TWFName = std::string; // workflow name 
    using TCat = int;
    using tDestination = std::string;
    using TPart = std::vector<TCat>;
    using TComb = std::uint64_t;
    using WorkflowRange = std::vector<Range>; // each of the 4 categories is represented by a vector of Range objects

    std::unordered_map<char,size_t> cat_map{ {'x',0},{'m',1},{'a',2},{'s',3} };

    struct Rule
    {
        tDestination dest{};
        std::string rule{};
    };

    struct Workflow
    {
        TWFName name{};
        std::vector<Rule> rules{};
    };

    struct Range
    {
        int start{};
        int end{};
        bool operator<(const Range &r){
            if (start != r.start) return start < r.start;
            else return end < r.end;
        }
    };

    std::pair<std::unordered_map<TWFName,Workflow>, std::vector<TPart>> get_data_in(const std::string &file_path);
    std::vector<TPart> get_accepted_parts(const std::unordered_map<TWFName,Workflow> &workflow_map, const std::vector<TPart> &parts);
    bool process_part(const std::unordered_map<TWFName,Workflow> &workflow_map, const TPart &part);
    std::pair<bool,tDestination> check_rule(const Rule &rule, const TPart &part);
    TComb get_accept_comb(const std::unordered_map<TWFName,Workflow> &workflow_map);
    std::pair<TComb,WorkflowRange> intersect_rule(const std::unordered_map<TWFName,Workflow> &workflow_map, const Rule &rule, const WorkflowRange &range);

    TCat sol_19_1(const std::string &file_path)
    {
        auto data_in = get_data_in(file_path);
        std::unordered_map<TWFName,Workflow> workflow_map = data_in.first;
        std::vector<TPart> part_vec = data_in.second;
        std::vector<TPart> acc_parts = get_accepted_parts(workflow_map, part_vec);
        TCat sum_rating{ 0 };
        for (const auto &part : acc_parts)
        {
            sum_rating = std::accumulate(part.begin(), part.end(),sum_rating);
        }

        return sum_rating;
    }


    TComb sol_19_2(const std::string &file_path)
    {
        auto data_in = get_data_in(file_path);
        std::unordered_map<TWFName,Workflow> workflow_map = data_in.first;
        
        return get_accept_comb(workflow_map);
    }

    TComb get_comb(const WorkflowRange &range)
    {
        TComb comb{ 1 };
        for (const auto &r : range)
        {
            comb *= (r.end - r.start + 1); 
        }
        return comb;
    }

    std::pair<TComb,WorkflowRange> process_workflow(const std::unordered_map<TWFName,Workflow> &workflow_map, const Workflow &wf, const WorkflowRange &range)
    {
        TComb accepted{ 0 };
        WorkflowRange cur_range{ range };
        for (const auto &r : wf.rules)
        {
            auto acc_comb_outside = intersect_rule(workflow_map,r,cur_range);
            accepted += acc_comb_outside.first;
            cur_range = acc_comb_outside.second;
        }

        return { accepted, cur_range };
    }

    std::pair<TComb,WorkflowRange> intersect_rule(const std::unordered_map<TWFName,Workflow> &workflow_map, const Rule &rule, const WorkflowRange &range)
    {
        WorkflowRange inside{ range };
        WorkflowRange outside{ {0,0},{0,0},{0,0},{0,0} };

        // 1. set inside and outside ranges based on current range + rule
        if (rule.rule == "")
        {
            inside = range;
        }
        else
        {
            size_t cat = cat_map.at(rule.rule[0]);
            auto comp_val = std::stoi(rule.rule.substr(2));
            auto &cur_range = inside[cat];
            switch (rule.rule[1])
            {
            case '<':
                if (cur_range.start >= comp_val) 
                { // all remaining combinations are mapped to outside
                    outside = range;
                }
                else 
                {
                    if (cur_range.end < comp_val) inside = range;
                    else
                    {
                        // create two ranges that are split at comp_val
                        inside = range;
                        outside = range;
                        inside[cat].end = comp_val-1;
                        outside[cat].start = comp_val;
                    }
                }
                break;
            case '>':
                if (cur_range.end <= comp_val) 
                { // all remaining combinations are mapped to outside
                    outside = range;
                }
                else 
                {
                    if (cur_range.start > comp_val) inside = range;
                    else
                    {
                        // create two ranges that are split at comp_val
                        inside = range;
                        outside = range;
                        inside[cat].start = comp_val+1;
                        outside[cat].end = comp_val;
                    }
                }
                break;
            
            default:
                throw std::runtime_error("intersect_rule: Error unknown operator in: " + rule.rule);
                break;
            }
        }

        // 2. inside and outside ranges are set -> return if destination of rule is either "A" or "R"
        // or recurse further by being directed to the next workflow
        if (rule.dest == "A") 
        {
            return { get_comb(inside), outside };
        }
        else
        {
            if (rule.dest == "R")
            {
                return { 0,outside };
            }
            else 
            {
                auto nxt_workflow = workflow_map.at(rule.dest);
                // all of the "inside" combinations are tracked further down in the tree
                auto res = process_workflow(workflow_map, nxt_workflow,inside);

                // return all accepted combinations + the remaining unhandled ranges
                return { res.first, outside };
            }
        }
    }


    /*
    Use a recursive function process_workflow that starts with the "in" workflow and then iteratively processes
    each of its rule. If a rule targets another workflow the recursive process_workflow is called again
    */
    TComb get_accept_comb(const std::unordered_map<TWFName,Workflow> &workflow_map)
    {
        Range max_range{ 1,4000 };
        WorkflowRange start_range{ { max_range,max_range,max_range,max_range }};
        auto res = process_workflow(workflow_map, workflow_map.at("in"),start_range);

        return res.first;
    }

    std::pair<bool,tDestination> check_rule(const Rule &rule, const TPart &part)
    {
        if (rule.rule == "")
        {
            return { true, rule.dest };
        }
        else
        {
            size_t cat = cat_map.at(rule.rule[0]);
            auto comp_val = std::stoi(rule.rule.substr(2));
            switch (rule.rule[1])
            {
            case '<':
                if (part[cat] < comp_val) return { true,rule.dest };
                else return { false,rule.dest };
                break;
            case '>':
                if (part[cat] > comp_val) return { true,rule.dest };
                else return { false,rule.dest };
                break;
            
            default:
                throw std::runtime_error("check_rule: Error unknown operator in: " + rule.rule);
                break;
            }
        }
    }

    bool process_part(const std::unordered_map<TWFName,Workflow> &workflow_map, const TPart &part)
    {
        tDestination dest{ "in" };
        while (dest != "A" && dest != "R")
        {
            const auto workflow = workflow_map.at(dest);
            for (const auto &rule : workflow.rules)
            {
                auto res = check_rule(rule,part);
                if (res.first)
                {
                    dest = res.second;
                    break;
                }
            }
        }

        return dest == "A";
    }

    std::vector<TPart> get_accepted_parts(const std::unordered_map<TWFName,Workflow> &workflow_map, const std::vector<TPart> &parts)
    {
        std::vector<TPart> accepted_parts;

        for (const auto &part : parts)
        {
            if (process_part(workflow_map, part))
            {
                accepted_parts.push_back(part);
            }
        }

        return accepted_parts;
    }

    Workflow parse_workflow(const std::string &wf_str)
    {
        // example: qqz{s>2770:qs,m<1801:hdj,R}
        auto name_split = split_string(wf_str,"{");
        auto rule_split = split_string(name_split[1].substr(0,name_split[1].length()-1),",");
        Workflow wf{ };
        wf.name = name_split[0];
        for (const auto &rule : rule_split)
        {
            auto dest_split = split_string(rule,":");
            if (dest_split.size() == 1)
            {
                wf.rules.push_back({ dest_split[0], "" });
            }
            else
            {
                wf.rules.push_back({ dest_split[1],dest_split[0] });
            }
        }
        return wf;
    }

    TPart parse_part(const std::string &part_str)
    {
        auto tmp = part_str.substr(1,part_str.length()-2); // remove curly braces in the front and back
        auto category_split = split_string(tmp,",");
        TPart c_nums; 
        for (const auto &cat_str : category_split)
        {
            c_nums.push_back(std::stoi(cat_str.substr(2)));
        }

        return c_nums;
    }

    std::pair<std::unordered_map<TWFName,Workflow>, std::vector<TPart>> get_data_in(const std::string &file_path)
    {
        std::unordered_map<TWFName,Workflow> workflow_map;
        std::vector<TPart> part_vec;

        std::fstream input_file;
        input_file.open(file_path,std::ios::in);
        if (input_file.is_open())
        {
            std::string input_line;
            // parse workflows
            while(getline(input_file, input_line))
            {  
                if (input_line == "") break;
                auto res = parse_workflow(input_line);
                workflow_map[res.name] = res;
            }

            // parse parts
            while(getline(input_file, input_line))
            {  
                part_vec.push_back(parse_part(input_line));
            }
            input_file.close();
        }
        return { workflow_map, part_vec };
    }
}