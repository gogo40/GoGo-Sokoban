#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <queue>

using namespace std;

int N, M;

int dx[] = {-1,  1,  0,  0};
int dy[] = { 0,  0, -1,  1};


typedef pair<int, int> State;
typedef pair<int, State> pState;

vector<string> vs;
vector<vector<int> > vbox;
map<string, int> ids;


void print(vector<string>& s) {

	for (int i = 0; i < s.size(); ++i) {
		cout << s[i] << endl;
	}
	cout << endl;
}

string getStr(vector<string>& s) {
	string out = "";
	for (int i = 0; i < s.size(); ++i) {
		out += s[i];
	}
	return out;
}

vector<string> getVec(string& s) {
	vector<string> v(N);

	int n = 0;

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j){
			v[i]+= s[n];
			++n;
		}

	return v;
}

vector<int> pfx;
vector<int> pfy;

inline int abs(int a) { return a>0?a:-a; }

int h(vector<string>& s, int p, int id) {
	int d = 0;
	int x = p/M;
	int y = p%M;
	int nc = 0;

	for (int n = 0; n < vbox[id].size(); ++n) {
			int i = vbox[id][n] / M;
			int j = vbox[id][n] % M;
			if (s[i][j] == 'o') {
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


bool isDead(vector<string>& s, int id) {
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
		if (s[i][j] == 'o') {
			if ((s[i-1][j] == '#' and s[i][j+1] == '#') or
				(s[i+1][j] == '#' and s[i][j+1] == '#') or
				(s[i+1][j] == '#' and s[i][j-1] == '#') or
				(s[i-1][j] == '#' and s[i][j-1] == '#')
				) {
					ok = true;
					break;
				}
		}
	}
	return ok;
}



bool makeMove(vector<string>& s, int x, int y, int k) {

	int px = x + dx[k];
	int py = y + dy[k];

	if (not (px > -1 and px < N and py > -1 and py < M)) {
		return false;
	}

	switch (s[px][py]) {
		case '.': s[px][py] = 'o'; break;
		case 'x': s[px][py] = 'O'; break;
		default: return false;
	}

	return true;
}


int calcDist(vector<string>& s, int x, int y, int xf, int yf) {
	map<int, int> D;
	queue<int> Q;

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
				if (s[vx][vy] == '.' || s[vx][vy] == 'x') {
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

	vector<string> m = getVec(vs[u.first]);
	int po = u.second;
	int x = po/M;
	int y = po%M;

	m[x][y] = 'I';

	cout<< du - p<< endl;
	print(m);
}

void findBox(vector<int>& vb, vector<string>& s) {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			if (s[i][j] == 'o' || s[i][j] == 'O') {
				vb.push_back(i * M + j);
			}
}

int main()
{
	cin>>N>>M;

	vector<string> s(N), f(N);

	for (int i = 0; i < N; ++i) {
		cin>>s[i];
		f[i] = s[i];
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
			if (s[i][j] == 'I') {
				f[i][j] = s[i][j] = '.';
				po = i * M +j;
				xo = i;
				yo = j;
			}else if (s[i][j] == 'J') {
				s[i][j] = 'x';
				f[i][j] = 'O';
				po = i * M +j;
				xo = i;
				yo = j;
			} else if (s[i][j] == 'x') {
				f[i][j] = 'O';
				pfx.push_back(i);
				pfy.push_back(j);
			} else if (s[i][j] == 'o') {
				f[i][j] = '.';
			} else f[i][j] = s[i][j];


	vs.push_back(getStr(s));
	vbox.push_back(vector<int>());
	findBox(vbox[0], s);
	ids[getStr(s)] = 0;

	vs.push_back(getStr(f));
	vbox.push_back(vector<int>());
	findBox(vbox[1], f);
	ids[getStr(f)] = 1;

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


	for (int L = 140; L < 1000; L *= 2) {

		if (ok) break;


		priority_queue<pState> Q;
		D.clear();
		Q.push(pState(0, ini));
		D[ini] = 0;


		while (!Q.empty()) {
			int uest = -Q.top().first;
			State u = Q.top().second;

			if (u == fim){
				ok = true;
				//break;
			}

			int p = u.second;
			int du = D[u];
			int x = p/M;
			int y = p%M;

			string s = vs[u.first];

			Q.pop();

			if ( uest > L or uest >= dmin) continue;
			else if (u == fim){
				dmin = du;
				//break;
			}
			if (ck % 10000 == 0) {
				cerr << du << " dmin=" << dmin << endl;
				cerr << s << endl;
			}
			++ck;

			//break;

			vector<string> mu = getVec(s);

			for (int nb = 0; nb < vbox[u.first].size(); ++nb){
					int i = vbox[u.first][nb] / M;
					int j = vbox[u.first][nb] % M;
					if (mu[i][j] == 'o' || mu[i][j] == 'O') {
						int q = i * M + j;

						for (int k = 0; k < 4; ++k) {

							vector<string> m = getVec(s);

							if (makeMove(m, i, j, k)) {
								m[i][j] = (mu[i][j]=='o')?'.':'x';

								int xf = i - dx[k];
								int yf = j - dy[k];

								int dup = calcDist(mu, x, y, xf, yf);

								if (dup > -1) {
									//dup++;
									State v;
									string ms = getStr(m);

									if (ids.find(ms) == ids.end()) {
										ids[ms] = vs.size();
										vbox.push_back(vector<int>());
										findBox(vbox[vs.size()], m);
										vs.push_back(ms);
									}

									v.first = ids[ms];

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

