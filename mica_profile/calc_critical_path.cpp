#include "global_def.h"
#include "calc_critical_path.h"

/* 关键路径长度 */
void CriticalPath::calCriticalPath(const InstReg &instReg){ //10/12 debug is solved
	if (committedInstList.size() > (robSize - 1)) {
		committedInstList.clear();
		/* push the weight everytime with instReg */
		committedInstList.push_back(instReg);
		return;
	}
	else if (committedInstList.size() < (robSize - 1)){
		committedInstList.push_back(instReg);
		return;
	}
	else
		committedInstList.push_back(instReg);	// [LHX] 每凑够一组ROB大小的，就计算一次 。因为指令的重排序是在ROB模块中进行的，因此关键路径长度的最大值不会大于ROB大小。

	vector<InstReg>::iterator it;
	vector<InstReg>::iterator jt;
	int numIt = 0;
	int numJt = 0;

	/* vertex index starts at 1 */
	LongestDepGraph *dg = new LongestDepGraph(robSize + 1);

	// find the dependencies between dynamic insts. about 0.9ms/160 insts
	for (it = committedInstList.begin(); it != committedInstList.end(); ++it){								// [LHX] 双层循环查找源寄存器与目的寄存器相等的
		numIt++;
		/* by shen 10/22 */
		for (int i = 0; i < DREG_SIZE; i++){
			numJt = numIt;
			for (jt = it + 1; jt != committedInstList.end(); ++jt){
				numJt++;
				for (int j = 0; j < SREG_SIZE; j++){
					if (it->isDesLegel(i) && jt->isSrcLegel(j)) 
						if (jt->SReg[j] == it->DReg[i] && jt->isFpSrcReg(j) == it->isFpDesReg(i)){
							/* default latency of any unknown instruction is 1 */
							if (it->delay == 0) it->delay = 1;
							(*dg).addEdge(numIt, numJt, it->delay);
						}
				}
			}
		}
	}

	/* construct a dependency graph. about 0.8ms/160 insts */
	int maxLength = 0;

	/*call longestPath() to get the critical path */
	for (uint32_t s = 1; s < robSize + 1; s++){
		int temp = dg->longestPath(s);
		if (temp > maxLength){
			maxLength = temp;
		}
	}

	delete dg;

	if (maxLength > distSize - 1)
		maxLength = distSize - 1;
	/* statistic critical path length distribution */
	++criticalPathLength[maxLength];
}


void LongestDepGraph::addEdge(const int & u, const int & v, const int & weight){
	AdjListNode node(v, weight);
	adj[u].push_back(node);                                 // add v to u's vector
}

void LongestDepGraph::topologicalSortUtil(const int & v, bool *visited, stack<int> & Stack){
	visited[v] = true; // mark current node as visited

	//cout << v << " ";
	
	deque<AdjListNode>::iterator it;
	for (it = adj[v].begin(); it != adj[v].end(); ++it){
		AdjListNode node = *it;
		if (!visited[node.getV()])
			topologicalSortUtil(node.getV(), visited, Stack);
	}
	//cout << endl;

	Stack.push(v); // push current vertex to stack
}

int LongestDepGraph::longestPath(const int & s){
	stack<int> Stack;
	int *dist = new int[numV];                                          //V = N + 1 = 200 + 1

	bool *visited = new bool[numV];
	for (int i = 0; i < numV; i++){
		visited[i] = false;
	}

	for (int i = 0; i < numV; i++){
		if (visited[i] == false)
			topologicalSortUtil(i, visited, Stack);
	}

	for (int i = 0; i < numV; i++){
		dist[i] = NINF;
	}
	dist[s] = 0;

	while (Stack.empty() == false){
		int u = Stack.top();
		Stack.pop();

		deque<AdjListNode>::iterator it;
		if (dist[u] != NINF){
			//deque<AdjListNode> c = adj[u];
			//deque<int> a;
			//a.push_back(1);
			for (it = adj[u].begin(); it != adj[u].end(); ++it){
				//int a = dist[u];
				if (dist[it->getV()] < dist[u] + it->getWeight())
					dist[it->getV()] = dist[u] + it->getWeight();
			}
		}
	}
	delete[] visited;

	// return the longest distance
	int criticalPathLength = 0;
	int criticalPathInstNum = 0;
	for (int i = 0; i < numV; i++) {
		// (dist[i] == NINF)? std::cout << "INF ": std::cout << dist[i] << " ";
		if (dist[i] != NINF){
			if (criticalPathLength < dist[i])
				criticalPathLength = dist[i];// shen changed
			criticalPathInstNum += 1;
		}
	}
	//return criticalPathLength;
	delete[] dist;
	return criticalPathLength;
}