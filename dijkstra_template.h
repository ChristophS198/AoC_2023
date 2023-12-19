#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>

/**
 * \fn dijkstraFn<TDist,TNode>(firstnode, end_cond, for_all_neighbors_of, return_route)
 * \brief Searches for the shortest route between nodes `firstnode` and some end node (is found based on end_cond).
 *
 * \tparam TDist                Type of distances
 * \tparam TNode                Type of node indexes
 * \param firstnode             Node to start search from.
 * \param end_cond              Functor for the end condition
 * \param for_all_neighbors_of  Functor for enumerating neighbors and distances, see below.
 * \param return_route          Functor for receiving route information, see below.
 *
 * \remark \parblock
 * If `end_cond` is never fulfilled, the shortest route from `firstnode` to every reachable node is calculated.
 *
 * `end_cond` must be a functor: bool(TNode cur_node)
 * which is called in each iteration for the nxt node to be processed
 *
 * `for_all_neighbors_of` must be a functor: void(TNode source_node, Functor f)
 * which calls `f` with parameters (TNode target_node, TDist d)
 *   for all neighbors of `source_node`, where `target_node` is a neighbor of `source_node`,
 *                              and `d` is the distance from `source_node` to `target_node`.
 *
 * `return_route` must be a functor: void(TNode target, TNode source, TDist length),
 *   that will be called several times after the search is complete.
 *   `length` is the shortest total distance from `firstnode` node to the `target` node.
 *   `source` is the node through which the shortest route to `target` goes.
 *
 *   If 'end_cond' is fulfilled in first iteration or 'end_cond' is never fulfilled
 *     This function will be called for every target
 *     that is reachable from firstnode, in an unspecified order.
 *
 *   If `end_cond` is not fulfilled in the first iteration, but in a later iteration
 *     The function calls comprise the reverse route from `firstnode` to the node
 *       fulfilling 'end_cond',
 *       with first call having   `target` = `lastnode`,
 *       and the last call having `source` = `firstnode`.
 * \endparblock
 *
 * \returns Shortest total distance from `firstnode` to the node fulfilling 'end_cond',
 *          or a default-constructed TDist if 'end_cond' is never fulfilled or only in
 *          the first iteration
 */
template<
    typename TDist,
    typename TNode,
    typename EndCondition,
    typename NodeIterationFunction,
    typename RouteIterationFunction
>
TDist dijkstraFn(TNode firstnode,
                      EndCondition end_cond,
                      NodeIterationFunction&& for_all_neighbors_of,
                      RouteIterationFunction&& return_route)
{
    struct NodeInfo { TDist distance; TNode previous; bool visited; };

    std::unordered_map<TNode, NodeInfo> node_map;

    using dp = std::pair<TNode,TDist>;
    auto compare = [&](const dp& a, const dp& b) { return a.second > b.second; };
    std::priority_queue<dp, std::vector<dp>, decltype(compare)> queue(compare);

    // The priority queue elements must contain a copy of the distance,
    // because without the distance, modifying node_map[].distance may break
    // the heap property of the priority queue.

    // Begin from firstnode with blank distance
    node_map.emplace(firstnode, NodeInfo{});
    queue.emplace(firstnode, TDist{});
    TNode last_node{};
    while(!queue.empty())
    {
        // Find the node with shortest distance
        auto Upair = queue.top(); queue.pop();
        TNode U             = Upair.first;
        TDist Udistance = Upair.second;
        // U = node number, Uinfo.second = total distance from firstnode

        // Terminate if the end condition is reached,
        if(U != firstnode && end_cond(U)) 
        {
            last_node = U;
            break;
        }

        // Mark the node visited. Ignore the node if already visited before.
        auto cur_node = node_map.find(U);
        if(cur_node->second.visited) continue;
        cur_node->second.visited = true;

        // Check all neighbors_of of U that have not yet been visited.
        for_all_neighbors_of(U, [=,&node_map,&queue](TNode V, TDist distance)
        {
            distance += Udistance;
            // If V is previously unknown, or if V has not yet been visited and
            // the new distance is shorter than what is previously known for V,
            // update records and make sure that this target is eventually visited.
            auto neigh_node = node_map.find(V);
            if(neigh_node == node_map.end()) // Previously unknown
            {
                node_map.emplace(V,  NodeInfo{distance,U,false});
                queue.emplace(V, distance);
            }
            else if(!neigh_node->second.visited && neigh_node->second.distance > distance)
            {
                neigh_node->second.distance = distance;
                neigh_node->second.previous = U;
                queue.emplace(V, distance);
            }
        });
    }

    auto i = node_map.find(last_node);
    if(firstnode != last_node && i != node_map.cend())
    {
        // Report the route from last_node to firstnode. The visited flag is not used.
        for(auto j = i; j->first != firstnode; j = node_map.find(j->second.previous))
            return_route(j->first, j->second.previous, j->second.distance);
        return i->second.distance;
    }
    // Report all reachable routes.
    for(const auto& l: node_map)
        if(l.second.visited)
            return_route(l.first, l.second.previous, l.second.distance);
    return {};
}