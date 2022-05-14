/*
* ol方法3+时间种子，100调+aver1+30调
* 完美版，正赛295w
*/
#define _CRT_SECURE_NO_WARNINGS
#include<cstring>
#include<iostream>
#include<map>
#include<vector>
#include<algorithm>
#include <time.h>
using namespace std;
const int T = 9000, M = 40, N = 140, Max = 1e5 + 5;
map<string, int> m_id1; // 客户ID转化为对应数字
map<int, string> m_id2; // 对应数字转化为客户ID 
map<string, int> n_id1; // 节点ID转化为对应数字
map<int, string> n_id2; // 对应数字转化为节点ID 
string name[M];
const int averge_time = 50;
int id1; // 客户总数   小40
int id2; // 服务器总数   大140
int t; // 总天数
char s[Max]; // 输入用的 
int demand[T][M]; // 需求
int limit[N]; // 带宽上限
int used[T][N]; // 使用了的带宽 
int q[N][N]; // 时延 记得改！！！
int Qos; // 时延上限 
int day_th;//每日满载服务器
int ser_th;//服务器满载上线
bool full_server_index[T][N];//日满载服务器id
int specific;//95的时间节点
long int past_money;
long int client_cost[M];

struct wfh {
	int num[N]; // 分配的数量 
}out[T][M]; // 存储输出
struct elaina1 {
	int index;
	int occupy;
	int total_handle_demand;//最大总需求 每日更新
}ser_seq1[N][T];//找出95点的值
struct elaina2 {//不排序
	int index;
	int occupy;
	int node_bandwith[M];//来自各节点的流量	
}ser_seq[N][T];//调度用
struct elaina3 {
	int index;
	int assign;
}client[M];//调度用
struct elaina5 {
	int index;
	int rescouce;//可用资源
	int demand;
}client1[T][M];
struct elaina4 {
	int index;
	int assign;
}day_pro[T];//调度用
struct elaina {
	int index;//原始序号
	int connect_num;//连接客户数量
	int total_handle_demand;//最大总需求 每日更新
	int full_times;//满载次数	
	bool connect_client[M];//连接到客户的集合  后续可改为动态		
} server[N];
/*struct elaina6 {  //排序
	int index;  //原始序号
	int client_cost_sum;//95点前所有客户的花费，在average2里用
	bool dra_connect_client[M];//动态连接到客户的集合  前95%
} server1[N];*/
struct elaina7 {
	int index;
	int has_cost95;//记录95点后的开销  overload用
	int rest_need;
}client2[M];//调度用
void makerand(elaina A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
void makerand(elaina3 A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina3 temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
void makerand(elaina4 A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina4 temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
/*void makerand(elaina6 A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina6 temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}*/
void makerand(elaina5 A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina5 temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
void init()
{
	day_th = id2 * 0.05;//参数
	ser_th = t * 0.05;
	//ser_th = t * 0.05 - 0.999 < id2 ? t * 0.05 - 0.999 : id2;//照理来说上面更优化，但上面跑了190w
	for (int i = 1; i <= id2; i++)
	{
		server[i].index = i;
		//server1[i].index = i;
		for (int j = 1; j <= id1; j++)
		{
			if (q[i][j] < Qos)
			{
				server[i].connect_num++;
				server[i].connect_client[j] = true;
				client1[i][j].rescouce += limit[i];
			}
		}
	}
	specific = t * 0.95 + 0.999;
	for (int i = 1; i <= t; i++)
	{
		day_pro[i].index = i;
	}
	for (int i = 1; i <= id1; i++)
	{
		client[i].index = i;
		client2[i].index = i;
	}
	for (int i = 1; i <= t; i++)
	{
		for (int j = 1; j <= id1; j++)
		{
			day_pro[i].assign += demand[i][j];
			client1[i][j].index = j;
		}
	}
	for (int j = 1; j <= id2; j++)
	{
		for (int i = 1; i <= t; i++)
		{
			ser_seq1[j][i].index = j;
			ser_seq[j][i].index = j;
		}
	}
	for (int j = 1; j <= id1; j++)
	{
		for (int i = 1; i <= t; i++)
		{
			client2[j].rest_need += demand[i][j];
		}
	}
}
bool cmp(elaina A, elaina B) { return A.connect_num < B.connect_num; }
bool cmp1(elaina3 A, elaina3 B) { return A.assign < B.assign; }
bool cmp2(elaina1 A, elaina1 B) { return A.occupy < B.occupy; }
bool cmp3(elaina4 A, elaina4 B) { return A.assign > B.assign; }
bool cmp4(elaina4 A, elaina4 B) { return A.index < B.index; }
bool cmp5(elaina5 A, elaina5 B) { return A.rescouce < B.rescouce; }
//bool cmp6(elaina6 A, elaina6 B) { return A.index < B.index; }
//bool cmp7(elaina6 A, elaina6 B) { return A.client_cost_sum < B.client_cost_sum; }
bool cmp8(elaina7 A, elaina7 B) { return A.has_cost95 < B.has_cost95; }
bool cmp9(elaina5 A, elaina5 B) { return A.demand > B.demand; }
bool cmp10(elaina7 A, elaina7 B) { return A.rest_need > B.rest_need; }
void output()
{
	//for (int i = 1; i <= id2; i++)
		//cout << n_id2[server[i].index] << " full times:    " << server[i].full_times << "    connects:    " << server[i].connect_num << endl;
	wfh A; bool flag;
	for (int i = 1; i <= t; i++)
		for (int j = 1; j <= id1; j++)
		{
			flag = 0;
			cout << m_id2[j] << ":";
			A = out[i][j];
			for (int k = 1; k <= id2; k++)
			{
				if (A.num[k] == 0) continue;
				if (flag == 1) cout << ",";
				cout << "<" << n_id2[k] << "," << A.num[k] << ">";
				flag = 1;
			}
			cout << endl;
		}
}
void overload1()
{
	sort(server + 1, server + 1 + id2, cmp);
	for (int s = 1; s <= id2; s++)
	{
		for (int time = 0; time < ser_th; time++)
		{
			int demand_max = 0;
			int day_index = 0;
			for (int i = 1; i <= t; i++)
			{
				ser_seq1[server[s].index][i].total_handle_demand = 0;
			}
			for (int i = 1; i <= t; i++)
			{
				for (int k = 1; k <= id1; k++)
				{
					if (q[server[s].index][k] < Qos && limit[server[s].index] - used[i][server[s].index] * 1.1>0 && server[server[s].index].full_times < ser_th)
					{
						ser_seq1[server[s].index][i].total_handle_demand += demand[i][k];
					}
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (full_server_index[i][server[s].index] == true && server[server[s].index].full_times >= ser_th)
				{
					ser_seq1[server[s].index][i].total_handle_demand = 0;
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (ser_seq1[server[s].index][i].total_handle_demand > demand_max)//可能得加参数？
				{
					demand_max = ser_seq1[server[s].index][i].total_handle_demand;
					day_index = i;
				}
			}
			for (int i = 1; i <= id1; i++)
			{
				client1[day_index][i].demand = demand[day_index][i];
			}
			if (demand_max > 0)
			{
				full_server_index[day_index][server[s].index] = true;
				server[server[s].index].full_times++;
				if (demand_max <= limit[server[s].index])
				{
					for (int o = 1; o <= id1; o++)
					{
						if (q[server[s].index][client2[o].index] < Qos)
						{
							client2[o].rest_need -= demand[day_index][client2[o].index];
							client2[o].has_cost95 += demand[day_index][client2[o].index];
							ser_seq[server[s].index][day_index].node_bandwith[o] += demand[day_index][client2[o].index];
							ser_seq[server[s].index][day_index].occupy += demand[day_index][client2[o].index];
							out[day_index][client2[o].index].num[server[s].index] += demand[day_index][client2[o].index];
							used[day_index][server[s].index] += demand[day_index][client2[o].index];
							demand[day_index][client2[o].index] -= demand[day_index][client2[o].index];
						}
					}
				}
				else
				{
					//两种思路，一是按95点后花销又小到大排，二是按当日花销由大到小排，在模拟集上测试前面的比较优秀
					//sort(client2 + 1, client2 + 1 + id1, cmp8);
					//sort(client1[day_index] + 1, client1[day_index] + 1 + id1, cmp9);
					sort(client2 + 1, client2 + 1 + id1, cmp10);
					for (int o = 1; o <= id1; o++)
					{
						if (q[server[s].index][client2[o].index] < Qos)
						{
							int a = demand[day_index][client2[o].index] < limit[server[s].index] - used[day_index][server[s].index] ? demand[day_index][client2[o].index] : limit[server[s].index] - used[day_index][server[s].index];
							if (a == 0) continue;
							client2[o].rest_need -= a;
							client2[o].has_cost95 += a;
							ser_seq[server[s].index][day_index].occupy += a;
							ser_seq[server[s].index][day_index].node_bandwith[client2[o].index] += a;
							out[day_index][client2[o].index].num[server[s].index] += a;
							used[day_index][server[s].index] += a;
							demand[day_index][client2[o].index] -= a;
						}
					}
				}
			}
		}
	}
}
void handle_money()
{
	for (int i = 1; i <= t; i++)//day
	{
		//sort(client1[i] + 1, client1[i] + 1 + id1, cmp5);
		int sum = 0;
		for (int j = 1; j < id1; j++)
		{
			sum += demand[i][j];
		}
		int aver = (sum / id2 / 1000) > 1 ? (sum / id2 / 1000) : 1;
		for (int j = 1; j <= id1; j++)//client
		{
			if (demand[i][client[j].index] == 0) continue;
			while (demand[i][client[j].index] > 0)
			{
			L:
				int ser_num = rand() % id2 + 1;
				if (q[ser_num][client[j].index] >= Qos || limit[ser_num] <= used[i][ser_num]) goto L;
				if (demand[i][client[j].index] >= aver)//分配aver
				{
					if (limit[ser_num] - used[i][ser_num] >= aver)
					{
						ser_seq[ser_num][i].occupy += aver;
						ser_seq[ser_num][i].node_bandwith[client[j].index] += aver;
						out[i][client[j].index].num[ser_num] += aver;
						used[i][ser_num] += aver;
						demand[i][client[j].index] -= aver;
					}
					else if (limit[ser_num] - used[i][ser_num] < aver)
					{
						ser_seq[ser_num][i].occupy += limit[ser_num] - used[i][ser_num];
						ser_seq[ser_num][i].node_bandwith[client[j].index] += limit[ser_num] - used[i][ser_num];
						out[i][client[j].index].num[ser_num] += limit[ser_num] - used[i][ser_num];
						demand[i][client[j].index] = demand[i][client[j].index] - (limit[ser_num] - used[i][ser_num]);
						used[i][ser_num] = limit[ser_num];
					}
				}
				else if (demand[i][client[j].index] < aver)//分配剩余需求
				{
					if (limit[ser_num] - used[i][ser_num] >= demand[i][client[j].index])
					{
						ser_seq[ser_num][i].occupy += demand[i][client[j].index];
						ser_seq[ser_num][i].node_bandwith[client[j].index] += demand[i][client[j].index];
						out[i][client[j].index].num[ser_num] += demand[i][client[j].index];
						used[i][ser_num] += demand[i][client[j].index];
						demand[i][client[j].index] = 0;
					}
					else if (limit[ser_num] - used[i][ser_num] < demand[i][client[j].index])
					{
						ser_seq[ser_num][i].occupy += limit[ser_num] - used[i][ser_num];
						ser_seq[ser_num][i].node_bandwith[client[j].index] += limit[ser_num] - used[i][ser_num];
						out[i][client[j].index].num[ser_num] += limit[ser_num] - used[i][ser_num];
						demand[i][client[j].index] = demand[i][client[j].index] - (limit[ser_num] - used[i][ser_num]);
						used[i][ser_num] = limit[ser_num];
					}
				}
			}
		}
	}
}
void calculate95()
{
	for (int j = 1; j <= id2; j++)
	{
		for (int i = 1; i <= t; i++)
		{
			ser_seq1[j][i].occupy = ser_seq[j][i].occupy;
		}
		sort(ser_seq1[j] + 1, ser_seq1[j] + 1 + t, cmp2);//可改为冒泡提升效率  时间轴打乱
	}
}
void average()
{
	calculate95();
	int x = 0;
	makerand(server, id2);
	//sort(server + 1, server + 1 + id2, cmp);
	//sort(day_pro + 1, day_pro + 1 + t, cmp4);
	//makerand(day_pro, t);
	for (int i = 1; i <= t; i++)
	{
		for (int k = 1; k <= id1; k++)
		{
			for (int j = 1; j <= id2; j++)
			{
				if (ser_seq[j][day_pro[i].index].occupy <= ser_seq1[j][specific].occupy)
				{
					client[k].assign += ser_seq[j][day_pro[i].index].node_bandwith[k];
				}
			}
		}
		//sort(client + 1, client + 1 + id1, cmp1);
		makerand(client, id1);
		for (int j = 1; j <= id1; j++)
		{
			for (int k = 1; k < id2; k++)
			{
				if ((q[server[k].index][client[j].index] < Qos)
					&& (ser_seq1[server[k].index][specific].occupy >= ser_seq[server[k].index][day_pro[i].index].occupy)
					&& (ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index] > 0))
				{
					for (int o = k + 1; o <= id2; o++)
					{
						if ((q[server[o].index][client[j].index] < Qos)
							&& ser_seq1[server[o].index][specific].occupy != ser_seq[server[o].index][day_pro[i].index].occupy)
						{
							x = 0;
							if (ser_seq1[server[o].index][specific].occupy > ser_seq[server[o].index][day_pro[i].index].occupy)
							{
								if (ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index] + ser_seq[server[o].index][day_pro[i].index].occupy < ser_seq1[server[o].index][specific].occupy)
								{
									x = ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index];
								}
								else
								{
									x = ser_seq1[server[o].index][specific].occupy - ser_seq[server[o].index][day_pro[i].index].occupy;
								}
							}
							else
							{
								if (ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index] + ser_seq[server[o].index][day_pro[i].index].occupy < limit[server[o].index])
								{
									x = ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index];
								}
								else
								{
									x = limit[server[o].index] - ser_seq[server[o].index][day_pro[i].index].occupy;
								}
							}
							out[day_pro[i].index][client[j].index].num[server[k].index] -= x;
							out[day_pro[i].index][client[j].index].num[server[o].index] += x;
							ser_seq[server[o].index][day_pro[i].index].occupy += x;
							ser_seq[server[o].index][day_pro[i].index].node_bandwith[client[j].index] += x;
							ser_seq[server[k].index][day_pro[i].index].occupy -= x;
							ser_seq[server[k].index][day_pro[i].index].node_bandwith[client[j].index] -= x;
						}
					}
				}
			}
		}
	}

}
void average1()
{
	bool bigflag = true;
	while (bigflag)
	{
		bigflag = false;
		sort(server + 1, server + 1 + id1, cmp);
		for (int i = 1; i <= id2; i++)
		{
			for (int j = 1; j <= id2; j++)
			{
				if (server[i].connect_num < server[j].connect_num)
				{
					bool flag = true;
					for (int k = 1; k <= id1; k++)
					{
						if (server[i].connect_client[k] == true && server[j].connect_client[k] == false)
						{
							flag = false;
						}
					}
					if (flag)
					{
						int min = limit[server[j].index];
						for (int k = 1; k <= t; k++)
						{
							if (limit[server[j].index] - ser_seq[server[j].index][k].occupy < min)
							{
								min = limit[server[j].index] - ser_seq[server[j].index][k].occupy;
							}
						}
						for (int k = 1; k <= t && min != 0; k++)
						{
							if (full_server_index[k][server[i].index] == false)
							{
								for (int o = 1; o <= id1; o++)
								{
									int a = ser_seq[server[i].index][k].node_bandwith[o] < min ? ser_seq[server[i].index][k].node_bandwith[o] : min;
									if (a != 0)
									{
										out[k][o].num[server[i].index] -= a;
										out[k][o].num[server[j].index] += a;
										ser_seq[server[j].index][k].occupy += a;
										ser_seq[server[j].index][k].node_bandwith[o] += a;
										ser_seq[server[i].index][k].occupy -= a;
										ser_seq[server[i].index][k].node_bandwith[o] -= a;
										bigflag = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
/*void average2()
{
	//统计95前各用户的花费
	calculate95();
	for (int l = 1; l <= id1; l++)
	{
		client_cost[l] = 0;
	}
	for (int l = 1; l <= id2; l++)
	{
		for (int i = 1; i <= id1; i++)
		{
			server1[l].dra_connect_client[i] = false;
		}
	}
	for (int j = 1; j <= id2; j++)
	{
		for (int k = 1; k <= t; k++)
		{
			if (ser_seq[j][k].occupy > ser_seq1[j][specific].occupy)//sadasdas
			{
				//server1[j].client_cost_sum += ser_seq[j][k].occupy;
				for (int l = 1; l <= id1; l++)
				{
					if (ser_seq[j][k].node_bandwith[l] != 0)
					{
						client_cost[l] += ser_seq[j][k].node_bandwith[l];
						for (int i = 1; i <= id2; i++)
							if (server1[i].index == j)
								server1[i].dra_connect_client[l] = true;
					}
				}
			}
		}
	}
	//调度方向：是按 95点前总花销大的 还是 连接95点前有花销客户数多的？
	for (int j = 1; j <= id2; j++)
	{
		for (int k = 1; k <= id1; k++)
		{
			if (server1[j].dra_connect_client[k] == true)
			{
				server1[j].client_cost_sum += client_cost[k];
			}
		}

	}
	long sum = 0;
	for (int k = 1; k <= id1; k++)
	{
		cout<< m_id2[k] <<" : " << client_cost[k] << endl;
		sum += client_cost[k];
	}
	cout << sum << endl;
	sort(server1 + 1, server1 + 1 + id2, cmp7);
	//makerand(server1, id2);
	for (int fr = 1; fr <= id2 - 1; fr++)
	{
		for (int bc = fr + 1; bc <= id2; bc++)
		{
			for (int k = 1; k <= id1; k++)
			{
				if (server1[fr].dra_connect_client[k] == true && server1[bc].dra_connect_client[k] == true)
				{
					int min_back = limit[server1[bc].index];
					int min_front = 0;
					int max_b_ser_cost = 0; //把能转的转后剩余部分的值
					for (int l = 1; l <= t; l++)
					{
						if (ser_seq[server1[fr].index][l].occupy <= ser_seq1[server1[fr].index][specific].occupy)
						{
							int temp = ser_seq[server1[fr].index][l].occupy;
							for (int i = 1; i <= id1; i++)
							{
								if (server1[fr].dra_connect_client[i] == true && server1[bc].dra_connect_client[i] == true)
								{
									temp -= ser_seq[server1[fr].index][l].node_bandwith[i];
								}
							}
							if (limit[server1[bc].index] - ser_seq[server1[bc].index][l].occupy < min_back && temp < ser_seq[server1[fr].index][l].occupy)
								min_back = limit[server1[bc].index] - ser_seq[server1[bc].index][l].occupy;
							if (temp > max_b_ser_cost)
							{
								max_b_ser_cost = temp;
								min_front = ser_seq[server1[fr].index][l].occupy - temp;
							}
							else if (temp == max_b_ser_cost)
							{
								if (min_front < ser_seq[server1[fr].index][l].occupy - temp)
									min_front = ser_seq[server1[fr].index][l].occupy - temp;
							}
						}
					}
					int transfer = min_front < min_back ? min_front : min_back;
					int had_transfer = 0;
					for (int i = 1; i <= t; i++)
					{
						int had_transfer = 0;
						for (int l = 1; l <= id1 && transfer > had_transfer; l++)
						{
							if (server1[fr].dra_connect_client[l] == true && server1[bc].dra_connect_client[l] == true)
							{
								if (had_transfer + ser_seq[server1[fr].index][i].node_bandwith[l] <= transfer)
								{
									out[i][l].num[server1[fr].index] -= ser_seq[server1[fr].index][i].node_bandwith[l];
									out[i][l].num[server1[bc].index] += ser_seq[server1[fr].index][i].node_bandwith[l];
									ser_seq[server1[bc].index][i].node_bandwith[l] += ser_seq[server1[fr].index][i].node_bandwith[l];
									ser_seq[server1[bc].index][i].occupy += ser_seq[server1[fr].index][i].node_bandwith[l];
									ser_seq[server1[fr].index][i].occupy -= ser_seq[server1[fr].index][i].node_bandwith[l];
									had_transfer += ser_seq[server1[fr].index][i].node_bandwith[l];
									ser_seq[server1[fr].index][i].node_bandwith[l] = 0;
									if (q[server1[bc].index][l] >= Qos)
										cout << n_id2[server1[bc].index] << " : " << m_id2[l] << endl;
								}
								else
								{
									int ppp = transfer - had_transfer < ser_seq[server1[fr].index][i].node_bandwith[l] ? transfer - had_transfer : ser_seq[server1[fr].index][i].node_bandwith[l];
									out[i][l].num[server1[fr].index] -= ppp;
									out[i][l].num[server1[bc].index] += ppp;
									ser_seq[server1[bc].index][i].node_bandwith[l] += ppp;
									ser_seq[server1[bc].index][i].occupy += ppp;
									ser_seq[server1[fr].index][i].occupy -= ppp;
									had_transfer += ppp;
									ser_seq[server1[fr].index][i].node_bandwith[l] = ser_seq[server1[fr].index][i].node_bandwith[l] - ppp;
									if (q[server1[bc].index][l] >= Qos)
										cout << n_id2[server1[bc].index] << " : " << m_id2[l] << endl;
								}
							}
						}
					}
					calculate95();
					break;
				}
			}
		}
	}
}*/
int transf(int x, int y)
{
	int A = 0;
	for (int i = x; i <= y; i++)
		A = A * 10 + (s[i] - '0');
	return A;
}
void handle_id1()
{
	string A = "";
	scanf("%s", s + 1);
	int n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
	{
		if (s[i] == ',')
		{
			++id1;
			while (s[++i] != ',' && i <= n) A += s[i];
			m_id1[A] = id1;
			m_id2[id1] = A;
			A = "";
			--i;
		}
	}
}
void handle_client()
{
	int tot;
	while (scanf("%s", s + 1) != EOF)
	{
		++t; tot = 0;
		int n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				++i;
				for (int j = i + 1; j <= n; j++)
				{
					if (s[j] == ',')
						demand[t][++tot] = transf(i, j - 1), i = j + 1;
					if (j == n)
						demand[t][++tot] = transf(i, j), i = j + 1;
				}
			}
		}
	}
}
void handle_node()
{
	scanf("%s", s + 1);
	string A = "";
	while (scanf("%s", s + 1) != EOF)
	{
		A = ""; ++id2;
		int n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				n_id1[A] = id2;
				n_id2[id2] = A;
				limit[id2] = transf(i + 1, n);
				break;
			}
			A += s[i];
		}
	}
}
void handle_qos()
{
	int n; int A = 0; int B = 0; int S;
	string AI = "";
	scanf("%s", s + 1);
	n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
	{
		if (s[i] == ',')
		{
			S = i + 1;
			break;
		}
	}
	for (int i = S; i <= n; i++)
	{
		if (s[i] == ',') name[++A] = AI, AI = "";
		if (i == n) AI += s[i], name[++A] = AI;
		if (s[i] != ',') AI += s[i];
	}
	A = 0;
	B = 0;
	while (scanf("%s", s + 1) != EOF)
	{
		AI = "";
		B = 0;
		n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				for (int j = 1; j <= i - 1; j++)
					AI += s[j];
				break;
			}
		}
		A = n_id1[AI];
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				++i;
				for (int j = i + 1; j <= n; j++)
				{
					if (s[j] == ',')
						q[A][m_id1[name[++B]]] = transf(i, j - 1), i = j + 1;
					if (j == n)
						q[A][m_id1[name[++B]]] = transf(i, j), i = j + 1;
				}
			}
		}
	}
}
void handle_config()
{
	scanf("%s", s + 1);
	scanf("%s", s + 1);
	int n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
		if (s[i] == '=')
			Qos = transf(i + 1, n);
}
void overload2()
{
	sort(server + 1, server + 1 + id2, cmp);
	for (int s = 1; s <= id2; s++)
	{
		for (int time = 0; time < ser_th; time++)
		{
			int demand_max = 0;
			int day_index = 0;
			for (int i = 1; i <= t; i++)
			{
				ser_seq1[server[s].index][i].total_handle_demand = 0;
			}
			for (int i = 1; i <= t; i++)
			{
				for (int k = 1; k <= id1; k++)
				{
					if (q[server[s].index][k] < Qos && limit[server[s].index] - used[i][server[s].index] * 1.1>0 && server[server[s].index].full_times < ser_th)
					{
						ser_seq1[server[s].index][i].total_handle_demand += demand[i][k];
					}
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (full_server_index[i][server[s].index] == true && server[server[s].index].full_times >= ser_th)
				{
					ser_seq1[server[s].index][i].total_handle_demand = 0;
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (ser_seq1[server[s].index][i].total_handle_demand > demand_max)//可能得加参数？
				{
					demand_max = ser_seq1[server[s].index][i].total_handle_demand;
					day_index = i;
				}
			}
			//cout<< demand_max <<endl;
			if (demand_max > 0)
			{
				full_server_index[day_index][server[s].index] = true;
				server[server[s].index].full_times++;
				if (demand_max <= limit[server[s].index])
				{
					for (int o = 1; o <= id1; o++)
					{
						if (q[server[s].index][o] < Qos)
						{
							ser_seq[server[s].index][day_index].node_bandwith[o] += demand[day_index][o];
							ser_seq[server[s].index][day_index].occupy += demand[day_index][o];
							out[day_index][o].num[server[s].index] += demand[day_index][o];
							used[day_index][server[s].index] += demand[day_index][o];
							demand[day_index][o] -= demand[day_index][o];
							//ser_seq[server[s].index][day_index].dra_connect_client[o] = true;
						}
					}
				}
				else
				{
					sort(client1[day_index] + 1, client1[day_index] + 1 + id1, cmp5);
					for (int o = 1; o <= id1; o++)
					{
						if (q[server[s].index][o] < Qos)
						{
							int a = demand[day_index][client[o].index] < limit[server[s].index] - used[day_index][server[s].index] ? demand[day_index][client[o].index] : limit[server[s].index] - used[day_index][server[s].index];
							if (a == 0) break;
							ser_seq[server[s].index][day_index].occupy += a;
							ser_seq[server[s].index][day_index].node_bandwith[o] += a;
							out[day_index][o].num[server[s].index] += a;
							used[day_index][server[s].index] += a;
							demand[day_index][o] -= a;
							//ser_seq[server[s].index][day_index].dra_connect_client[o] = true;
						}
					}
				}
			}
		}
	}
}
int main()
{
	freopen("data/demand.csv", "r", stdin);
	handle_id1(); // 处理第一行 
	handle_client(); // 存储客户需求 
	fclose(stdin);

	freopen("data/site_bandwidth.csv", "r", stdin);
	handle_node(); // 存储节点带宽上限
	fclose(stdin);

	freopen("data/qos.csv", "r", stdin);
	handle_qos(); // 顺序存储时延，若到时顺序不与前面一致，则修改
	fclose(stdin);

	freopen("data/config.ini", "r", stdin);
	handle_config(); // 读时延上限 
	fclose(stdin);

	//输入完成，算法部分开始 
	freopen("output/solution.txt", "w", stdout);
	srand((unsigned int)time(NULL)); //随机数种子;  (unsigned int)time(NULL)   6  1
	init();
	overload1();
	handle_money();
	for (int p = 1; p <= 1000; p++)
	{
		average();
	}
	average1();
	for (int p = 1; p <= 300; p++)
	{
		average();
	}
	output();
	fclose(stdout);
	return 0;
}