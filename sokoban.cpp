#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <algorithm>
#include <queue>
#include <ctime>
#include <cstdlib>

using namespace std;

/*
Estrutura de dados para representar um estado do problema
Alfabeto do jogo:
		'.' espaco vazio
		'#' parede
		'I' personagem
		'J' personagem no destino
		'x' destino caixa
		'o' caixa
		'O' caixa no destino
*/
static int mask[600];
static char rm[8];

void initCState() {
	mask['.'] = 0; rm[0] = '.';
	mask['#'] = 1; rm[1] = '#';
	mask['I'] = 2; rm[2] = 'I';
	mask['J'] = 3; rm[3] = 'J';
	mask['x'] = 4; rm[4] = 'x';
	mask['o'] = 5; rm[5] = 'o';
	mask['O'] = 6; rm[6] = 'O';

}

class cState {
	public:
	
		cState(const cState& s) {
			N = s.N;
			M = s.M;
			v = s.v;
		}
		
		cState(int N = 0, int M = 0)
		: N(N), M(M), v( (N*M)/10 + 1, 0) {}
		
		char get(int i, int j) {
			int k = i * M + j;
			int p = k / 10;
			int n = k % 10;
			int value = (v[p] >> (3 * n) ) & 7;
			
			return rm[value];
		}
		
		void insert(int i, int j, char c) {
			int value = mask[c];
			int k = i * M + j;
			int p = k / 10;
			int n = k % 10;
			int w = v[p] & (7 << (3*n));
			v[p] ^= w;
			v[p] |= value << (3 * n); 
		}
		
		
		cState& operator=(const cState& x) {
			N = x.N;
			M = x.M;
			v = x.v;
			return *this;
		}
		
		void print() {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < M; ++j) {
					printf("%c", get(i, j));
				}
				printf("\n");
			}
		}
	
	friend bool operator<(const cState& a, const cState& b);
	friend bool operator==(const cState& a, const cState& b);
	
	bool operator()(const cState& b) {
		if (N > b.N or M > b.M) return false;
		if (*this == b) return false;
		
		for (int i = 0; i < v.size(); ++i) 
			if (v[i] > b.v[i]) {
				printf("false...\n");
				return false;
			}
		printf("true...\n");
		return true;
	}
	
	private:
	
		
		int N, M;
		
		vector<int> v;
};



bool operator==(const cState& a, const cState& b) {

	for (int i = 0; i < a.v.size(); ++i) 
		if (a.v[i] != b.v[i]) {
			return false;
		}

	return true;
}


bool operator<(const cState& a, const cState& b) {
	
	return lexicographical_compare(a.v.begin(), a.v.end(), b.v.begin(), b.v.end());
}

///////////////////////////////////////////////////////

int N, M;

int dx[] = {-1,  1,  0,  0};
int dy[] = { 0,  0, -1,  1};


typedef pair<int, int> State;
typedef pair<int, State> pState;

vector<cState> vs;
vector<vector<int> > vbox;
map<cState, int> ids;


void print(cState& s) {
	s.print();
	cout << endl;
}

string getStr(cState& s) {
	string out = "";
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			out += s.get(i, j);
	return out;
}

cState getVec(string& s) {
	cState v(N, M);

	int n = 0;

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j){
			v.insert(i, j, s[n]);
			++n;
		}

	return v;
}

vector<int> pfx;
vector<int> pfy;

inline int abs(int a) { return a>0?a:-a; }

int h(cState& s, int p, int id) {
	int d = 0;
	int x = p/M;
	int y = p%M;
	int nc = 0;

	for (int n = 0; n < vbox[id].size(); ++n) {
			int i = vbox[id][n] / M;
			int j = vbox[id][n] % M;
			if (s.get(i, j) == 'o') {
				int dv = 2*(N + M);
				for (int k = 0; k < pfx.size(); ++k) {
					int dx = abs(pfx[k] - i);
					int dy = abs(pfy[k] - j);
					int dp = (dx+dy);
					if (dp < dv) dv = dp;
				}
				d += dv;
				nc++;
			}
	}
	return d;
}


bool isDead(cState& s, int id) {
	bool ok = false;
/*
##
#o (i-1, j+1)

##
o# (i+1, j+1)


o#
## (i+1, j-1)

#o
## (i-1, j-1)
*/
	for (int n = 0; n < vbox[id].size(); ++n) {
		int i = vbox[id][n] / M;
		int j = vbox[id][n] % M;
		if (s.get(i, j) == 'o') {
			if ((s.get(i-1, j) == '#' and s.get(i, j+1) == '#') or
				(s.get(i+1, j) == '#' and s.get(i, j+1) == '#') or
				(s.get(i+1, j) == '#' and s.get(i, j-1) == '#') or
				(s.get(i-1, j) == '#' and s.get(i, j-1) == '#')
				) {
					ok = true;
					break;
				}
		}
	}
	return ok;
}



bool makeMove(cState& s, int x, int y, int k) {

	int px = x + dx[k];
	int py = y + dy[k];

	if (not (px > -1 and px < N and py > -1 and py < M)) {
		return false;
	}

	switch (s.get(px, py)) {
		case '.': s.insert(px, py, 'o'); break;
		case 'x': s.insert(px, py, 'O'); break;
		default: return false;
	}

	return true;
}


int calcDist(cState& s, int x, int y, int xf, int yf) {
	map<int, int> D;
	queue<int> Q;
	int ini, fim;
	ini = fim = 0;
	
	Q.push(x * M + y);
	
	D[x * M + y] = 0;

	while (not Q.empty()) {
		int u = Q.front();
		
		int du = D[u];
		int ux = u / M;
		int uy = u % M;

		if (ux  ==  xf and uy == yf) break;

		Q.pop();
		
		for (int k = 0; k < 4; ++k) {
			int vx = ux + dx[k];
			int vy = uy + dy[k];
			int v = vx * M +vy;

			if (vx > -1 and vx < N and vy > -1 and vy < M)
				if (s.get(vx, vy) == '.' || s.get(vx, vy) == 'x') {
					if (D.find(v) == D.end() || D[v] > du + 1) {
						D[v] = du + 1;
						Q.push(v);
					}
				}
		}
	}

	return D.find(xf * M + yf) == D.end()? -1 : D[xf * M + yf];
}

State ini, fim;
map<State, State> pi;

void print_sol(State& u, int p, int du) {
	if (u == ini) return;

	print_sol(pi[u], p + 1, du);

	if (u == fim) return;

	cState m = vs[u.first];
	int po = u.second;
	int x = po/M;
	int y = po%M;

	m.insert(x, y, 'I');

	cout<< du - p<< endl;
	print(m);
}

void findBox(vector<int>& vb, cState& s) {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			if (s.get(i, j) == 'o' || s.get(i, j) == 'O') {
				vb.push_back(i * M + j);
			}
}

int main()
{
	srand(time(NULL));
	initCState();
	
	cin>>N>>M;

	cState s(N, M), f(N, M);

	for (int i = 0; i < N; ++i) {
		string a;
		cin>>a;
		for (int j = 0; j < M; ++j) {
			f.insert(i, j, a[j]);
			s.insert(i, j, a[j]);
		}
	}
	
	/*
	Alfabeto do jogo:
		'.' espaco vazio
		'#' parede
		'I' personagem
		'J' personagem no destino
		'x' destino caixa
		'o' caixa
		'O' caixa no destino
	*/

	int xo, yo, po;

	pfx.clear();
	pfy.clear();

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			if (s.get(i, j) == 'I') {
				f.insert(i, j, '.');
				s.insert(i, j, '.');
				
				po = i * M +j;
				xo = i;
				yo = j;
			}else if (s.get(i, j) == 'J') {
				s.insert(i, j, 'x');
				f.insert(i, j, 'O');
				
				po = i * M +j;
				xo = i;
				yo = j;
			} else if (s.get(i, j) == 'x') {
				f.insert(i, j, 'O');
				
				pfx.push_back(i);
				pfy.push_back(j);
			} else if (s.get(i, j) == 'o') {
				f.insert(i, j, '.');
				
			} else {
				f.insert(i, j, s.get(i,j));
			}

	vs.push_back(s);
	vbox.push_back(vector<int>());
	findBox(vbox[0], s);
	ids[s] = 0;

	vs.push_back(f);
	vbox.push_back(vector<int>());
	findBox(vbox[1], f);
	ids[f] = 1;

	
	ini.first = 0;
	ini.second = po;

	fim.first = 1;
	fim.second = -1;

	pi.clear();

	map<State, int> D;

	D[ini] = 0;

	bool ok = false;
	int ck = 1;
	int dmin = 1<<20;


	for (int L = 250; L < 1000; L *= 2) {

		if (ok) break;


		priority_queue<pState> Q;
		D.clear();
		Q.push(pState(0, ini));
		D[ini] = 0;


		while (!Q.empty()) {
			int uest = -Q.top().first;
			State u = Q.top().second;
			int du = D[u];
			
			if (u == fim){
				ok = true;
				dmin = du;
				break;
			}

			int p = u.second;
			int x = p/M;
			int y = p%M;
			
			Q.pop();

			if ( uest > L or uest >= dmin) continue;
			else if (u == fim){
				dmin = du;
			}
			
			if (ck % 50000 == 0) {
				cerr << du << " dmin= " << dmin << endl;
			}
			++ck;
			

			for (int nb = 0; nb < vbox[u.first].size(); ++nb){
					int i = vbox[u.first][nb] / M;
					int j = vbox[u.first][nb] % M;
					if (vs[u.first].get(i, j) == 'o' || vs[u.first].get(i, j) == 'O') {
						int q = i * M + j;

						for (int k = 0; k < 4; ++k) {

							cState m = vs[u.first];
							
							if (makeMove(m, i, j, k)) {
								m.insert(i, j, (vs[u.first].get(i, j) =='o')?'.':'x');
								
								int xf = i - dx[k];
								int yf = j - dy[k];

								int dup = calcDist(vs[u.first], x, y, xf, yf);
								if (dup > -1) {
									State v;
									
									if (ids.find(m) == ids.end()) {
										v.first = ids[m] = vs.size();
										vbox.push_back(vector<int>());
										findBox(vbox[vs.size()], m);
										vs.push_back(m);
									} else v.first = ids[m];
									
									if (isDead(m, v.first)) continue;
									
									if (v.first == fim.first) q = -1;

									v.second = q;

									if (D.find(v) == D.end()) {
										D[v] = du + dup + 1;
										pi[v]= u;
										int est = du + dup + 1 + h(m, p, v.first);
										if (est < dmin and est < L) Q.push(pState(-est, v));
									} else if (D[v] > du + dup + 1) {
										D[v] = du + dup + 1;
										pi[v] = u;
										int est = du + dup + 1 + h(m, p, v.first);
										if (est < dmin and est < L) Q.push(pState(-est, v));
									}
								}
							}
						}
					}
			}
		}
	}

	if (ok) {
		print_sol(fim, 0, D[fim]);
	} else {
		cout << "[Nao ha solucao... =(]\n";
	}

	return 0;
}

