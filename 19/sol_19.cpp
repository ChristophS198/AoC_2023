#include <string>
#include <array>
#include <unordered_map>
#include <numeric>

#include "../utility.h"


namespace Day19
{

    using TWFName = std::string; // workflow name 
    using TCat = int;
    using tDestination = std::string;
    using TPart = std::vector<TCat>;

    std::unordered_map<char,size_t> cat_map{ {'x',0},{'m',1},{'a',2},{'s',3} };

    struct Rule
    {
        tDestination dest{};
        std::string rule{};
    };

    struct Workflow
    {
        TWFName name{};
        std::vector<Rule> rules;
    };

    std::pair<std::unordered_map<TWFName,Workflow>, std::vector<TPart>> get_data_in(const std::string &file_path);
    std::vector<TPart> get_accepted_parts(const std::unordered_map<TWFName,Workflow> &workflow_map, const std::vector<TPart> &parts);
    bool process_part(const std::unordered_map<TWFName,Workflow> &workflow_map, const TPart &part);
    std::pair<bool,tDestination> check_rule(const Rule &rule, const TPart &part);

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


    TCat sol_19_2(const std::string &file_path)
    {

        return 0;
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