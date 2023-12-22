#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>

#include "../utility.h"

namespace Day20
{
    class Module;
    using TModPtr = std::unique_ptr<Module>;

    constexpr char FlipFlopSymbol{ '%' };
    constexpr char ConjunctionSymbol{ '&' };

    enum EPulseType : bool
    {
        Low=0,
        High
    };
    struct Pulse
    {
        std::string src{}; 
        EPulseType type{};
        std::vector<std::string> targets{};
    };

    class Module
    {
    public:
        virtual Pulse process_pulse(const Pulse &pulse);
        virtual void add_input(const std::string &name) { ; }
        virtual void print_state() const { ; }
        Module(const std::string &n, const std::vector<std::string> &t) : name{n}, targets{t} {};
        virtual ~Module() = default;
    protected:
        std::string name{};
        std::vector<std::string> targets{};
    };

    class FlipFlop : public Module
    {
    public:
        FlipFlop(const std::string &n, const std::vector<std::string> &t) : Module(n,t) {};
        void print_state() const override { ; }
        void add_input(const std::string &name) override { ; }
        Pulse process_pulse(const Pulse &pulse) override;
        virtual ~FlipFlop() = default;
    private:
        bool state{ 0 }; // FlipFlops are initially in state off
    };

    class Conjunction : public Module
    {
    public:
        Conjunction(const std::string &n, const std::vector<std::string> &t) : Module(n,t) {};
        void add_input(const std::string &name) override { memory[name] = EPulseType::Low; }
        void print_state() const override;
        Pulse process_pulse(const Pulse &pulse) override;
        virtual ~Conjunction() = default;
    private:
        std::unordered_map<std::string,EPulseType> memory{ }; // Conjunction remembers the last pulse type for each input (init is low)
        int num_high{ 0 };
    };

    class ModuleFactory 
    {
    public: 
        virtual TModPtr create_module(const std::string &name,const std::vector<std::string> &targets);
        virtual ~ModuleFactory() = default;
    };

    struct PulseCnt{
        int LowCnt{};
        int HighCnt{};
    };

    class ModuleConfig
    {
    public:
        std::pair<PulseCnt,bool> get_pulse_num_after_button_press(size_t cnt);
        ModuleConfig(const std::vector<std::string> &data_in);
    private:
        std::unordered_map<std::string,TModPtr> mod_map;
    };

    int sol_20_1(const std::string &file_path)
    {
        auto data_in = read_string_vec_from_file(file_path);
        ModuleConfig mod_config(data_in);
        std::pair<PulseCnt,bool> pulse_nums{ {0,0}, false };
        for (int i=0; i<1000; ++i)
        {
            auto res = mod_config.get_pulse_num_after_button_press(i);
            pulse_nums.first.LowCnt += res.first.LowCnt;
            pulse_nums.first.HighCnt += res.first.HighCnt;
        }

        return pulse_nums.first.LowCnt * pulse_nums.first.HighCnt;
    }


    size_t sol_20_2(const std::string &file_path)
    {
        auto data_in = read_string_vec_from_file(file_path);
        ModuleConfig mod_config(data_in);
        PulseCnt pulse_nums{ 0,0 };
        size_t i{ 0ul };
        for (; i<1'000'000; ++i)
        {
            auto res = mod_config.get_pulse_num_after_button_press(i);
            if (res.second) 
            {
                break;
            }
        }

        return i;
    }


    std::pair<PulseCnt,bool> ModuleConfig::get_pulse_num_after_button_press(size_t cnt)
    {
        std::uint32_t num_low_pulses{ 0ul };
        std::uint32_t num_high_pulses{ 0ul };

        Pulse start_pulse{ "Button",EPulseType::Low,{ "broadcaster" } };
        std::queue<Pulse> unprocessed_pulses;
        unprocessed_pulses.push(start_pulse);
        bool rx_low_received{ false };

        while (!unprocessed_pulses.empty())
        {
            auto nxt_pulse = unprocessed_pulses.front();
            unprocessed_pulses.pop();

            std::string t{ "High" };
            if (nxt_pulse.type == EPulseType::Low) t = "Low";

            for (const auto &target : nxt_pulse.targets)
            {
                // std::cout << nxt_pulse.src << " " << t << " -> " << target << std::endl; 

                // increase respective pulse counter
                if (nxt_pulse.type == EPulseType::High) ++num_high_pulses;
                else ++num_low_pulses;

                if (target == "output") continue;
                if (target == "rx")
                {
                    rx_low_received = nxt_pulse.type == EPulseType::Low;
                    continue;
                }
                auto new_pulse = mod_map.at(target)->process_pulse(nxt_pulse);
                unprocessed_pulses.push(new_pulse);
                if ("nc" == target && nxt_pulse.type == EPulseType::High) 
                {
                    std::cout << cnt << std::endl;
                    mod_map.at("nc")->print_state();
                }

            }
        }

        return { { num_low_pulses,num_high_pulses }, rx_low_received };
    }

    Pulse Module::process_pulse(const Pulse &pulse)
    { 
        return { name, pulse.type,targets };
    }
    Pulse FlipFlop::process_pulse(const Pulse &pulse)
    {
        if (pulse.type == EPulseType::High) 
        {
            return {}; // no pulse will be transmitted since neither src nor any targets are set
        }
        else
        {
            Pulse out_pulse{ name, EPulseType::High, targets };
            state = !state; // flip state
            if (state == 0)
            { // if the state is switched from on to off -> transmit low pulse
                out_pulse.type = EPulseType::Low;
            }
            return out_pulse;
        }
    }
    Pulse Conjunction::process_pulse(const Pulse &pulse)
    {
        // 1. Update memory for current input and count of high mem entries
        // Note: if this src is not present in the memoryc, it is created with Low
        if (memory[pulse.src] != pulse.type)
        {
            memory[pulse.src] = pulse.type;
            if (pulse.type == EPulseType::High) ++num_high;
            else --num_high;
        }
        // 2. 
        if (num_high == memory.size()) return { name,EPulseType::Low,targets };
        else return { name,EPulseType::High,targets };
    }

    ModuleConfig::ModuleConfig(const std::vector<std::string> &data_in)
    {
        ModuleFactory factory{};
        std::unordered_map<std::string,std::vector<std::string>> target_src_map;
        for (const auto &mod_str : data_in)
        {
            auto in_out_split = split_string(mod_str," -> ");
            auto target_split = split_string(in_out_split[1],", ");
            auto name = in_out_split[0].substr(1);
            if ("broadcaster" == in_out_split[0]) name = in_out_split[0];
            std::vector<std::string> targets;
            for (const auto &t : target_split) 
            {
                targets.push_back(t);
                target_src_map[t].push_back(name);
            }
            mod_map[name] = factory.create_module(in_out_split[0],targets);
        }

        // create output module
        mod_map["output"] = std::make_unique<Module>("output",std::vector<std::string>{});

        // add memory entries in conjunction modules
        for (const auto &target : target_src_map)
        {
            // apparently some module targets are not defined as input
            auto it = mod_map.find(target.first);
            if (it != mod_map.end())
            {
                for (const auto &src : target.second)
                mod_map[target.first]->add_input(src);
            }
        }
    }

    TModPtr ModuleFactory::create_module(const std::string &name, const std::vector<std::string> &targets)
    {
        if ("broadcaster" == name)
        {
            return std::make_unique<Module>(name,targets);
        }
        else
        {
            switch (name[0])
            {
            case FlipFlopSymbol:
                return std::make_unique<FlipFlop>(name.substr(1),targets);
                break;
            case ConjunctionSymbol:
                return std::make_unique<Conjunction>(name.substr(1),targets);                
                break;
            default:
                throw std::runtime_error("create_module: invalid module type!");
                break;
            }
        }
    }

    void Conjunction::print_state() const 
    {
        for (const auto &elem : memory)
        {
            std::cout << elem.first << " - " << (elem.second==EPulseType::High) << std::endl;
        }
        std::cout << "\n";
    }

}
