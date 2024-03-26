#include<vector>
#include<functional>
#include<cmath>
double f1(double x) {
	if (x <= 0 && x >= -1)
		return (std::pow(x, 3) + 3 * std::pow(x, 2));
	else if (x >= 0 && x <= 1)
		return (-std::pow(x, 3) + 3 * std::pow(x, 2));
	throw std::exception("function is not defined");
}

double f12(double x) {
	return sin(x) / x;
}

class Spline {
private:
	std::function<double(double)> func;

	void calc_C() {
		C[0] = M1;
		C[n] = M2;
		std::vector<double> alpha_arr(n), beta_arr(n);
		alpha_arr[0] = 0;
		beta_arr[0] = M1;

		double A_i, B_i, C_i, phi_i;
		for (int i = 1; i < n; i++) {
			A_i = x_arr[i] - x_arr[i - 1];// элемент под диагональю
			C_i = -2 * (x_arr[i + 1] - x_arr[i - 1]);// диагональный элемент
			B_i = x_arr[i + 1] - x_arr[i];// наддиагональный элемент
			phi_i = -6 * ((f_arr[i + 1] - f_arr[i]) / (x_arr[i + 1] - x_arr[i]) - (f_arr[i] - f_arr[i - 1]) / (x_arr[i] - x_arr[i - 1]));

			alpha_arr[i] = B_i / (C_i - A_i * alpha_arr[i - 1]);
			beta_arr[i] = (A_i * beta_arr[i - 1] + phi_i) / (C_i - alpha_arr[i - 1] * A_i);
		}

		for (int i = n - 1; i >= 0; i--) {
			C[i] = (alpha_arr[i] * C[i + 1]) + beta_arr[i];
		}
	}
	void calc_A() {
		for (int i = 1; i < A.size() + 1; i++) {
			A[i - 1] = f_arr[i];
		}
	}
	void calc_B() {
		for (int i = 1; i < B.size() + 1; i++) {
			B[i - 1] = (f_arr[i] - f_arr[i - 1]) / (x_arr[i] - x_arr[i - 1]) + C[i] * (x_arr[i] - x_arr[i - 1]) / (3) + C[i - 1] * (x_arr[i] - x_arr[i - 1]) / (6);
		}
	}
	void calc_D() {
		for (int i = 1; i < D.size() + 1; i++) {
			D[i - 1] = (C[i] - C[i - 1]) / (x_arr[i] - x_arr[i - 1]);
		}
	}

	void calc_F() {
		for (auto e : x_N) {
			F.push_back(func(e));
		}
	}

	//!!! не нравится блок с производными
	void calc_dF() {
		double h = 0.001;
		for (int i = 0; i < x_N.size(); i++) {
			dF.push_back((func(x_N[i] + h) - func(x_N[i] - h)) / (2 * h));
		}
	}
	void calc_dS() {
		int spline_index = 1;
		for (auto e : x_N) {
			if (e > x_arr[spline_index]) {
				spline_index++;
			}
			else if (e < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
				spline_index--;
			}
			dS.push_back(B[spline_index - 1] + C[spline_index] * (e - x_N[spline_index]) + D[spline_index - 1] / 2 * std::pow((e - x_N[spline_index]), 2));
		}
	}
	void calc_d2F() { // вторая производная f'' = (f(x+h) -2f(x) + f(x-h))/h^2
		double h = 0.001;
		for (auto e : x_N)
			d2F.push_back((func(e + h) - 2 * func(e) + func(e - h)) / std::pow(h, 2));
	}
	void calc_d2S() { // вторая производная для сплайна ci *x + di(x - xi)
		int spline_index = 1;
		{
			int spline_index = 1;
			for (auto e : x_N) {
				if (e > x_arr[spline_index]) {
					spline_index++;
				}
				else if (e < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
					spline_index--;
				}
				d2S.push_back(C[spline_index] + D[spline_index - 1] * (e - x_N[spline_index]));
			}
		}
	}


	void calc_dif_F_S() {
		for (int i = 0; i < x_N.size(); i++) {
			dif_F_S.push_back(fabs(F[i] - S[i]));
		}
	}
	void calc_dif_dF_dS() {
		for (int i = 0; i < x_N.size(); i++) {
			dif_dF_dS.push_back(fabs(dF[i] - dS[i]));
		}
	}
	void calc_dif_d2F_d2S() {
		for (int i = 0; i < x_N.size(); i++) {
			dif_d2F_d2S.push_back(fabs(d2F[i] - d2S[i]));
		}
	}
public:
	//ПРОВЕРКА КОРРЕКТНОСТИ
	double max_dif_F_S, max_dif_dF_dS;
	double argmax_dif_F_S, argmax_dif_dF_dS;


	std::vector<double> F, dF, dS, d2F, d2S;
	std::vector<double> dif_F_S, dif_dF_dS;
	std::vector<double> dif_d2F_d2S;
	//ЗНАЧЕНИЯ КОНТРОЛЬНОЙ СЕТКИ
	std::vector<double> x_N, S;

	//ЗАДАЁТ ПОЛЬЗОВАТЕЛЬ
	int n;// число разбиений
	double M1, M2;// граничные условия
	std::vector<double> x_arr, f_arr; // сетка сплайна

	//ВЫЧИСЛЯЕТ ПРОГРАММА
	std::vector<double> A, B, C, D; //коэф-ы сплайна

	//КОНСТРУКТОРЫ
	Spline(std::vector<double> x, std::vector<double> f, double M1, double M2) :x_arr(x), f_arr(f), n(x.size() - 1), M1(M1), M2(M2), A(n), B(n), C(n + 1), D(n)
	{
		calc_C();
		calc_A();
		calc_D();
		calc_B();

	}
	Spline(int n, double a, double b, std::function<double(double)> func, double M1, double M2) :n(n), M1(M1), M2(M2), A(n), B(n), C(n + 1), D(n) {
		double h = (b - a) / (n);
		for (int i = 0; i <= n; i++) {
			x_arr.push_back(a + h * i);
			f_arr.push_back(func(a + h * i));
		}
		calc_C();
		calc_A();
		calc_D();
		calc_B();
	}
	//МЕТОДЫ КЛАССА

	// задание функции(делается для тестов) 
	// ЗАМЕЧАНИЕ: Spline не будет сохранять функцию, если она передаётся в какой-либо другой метод класса или конструктор
	std::pair<std::vector<double>, std::vector<double>> calculate(int n) {//n - число разбиений
		double h = (x_arr[x_arr.size() - 1] - x_arr[0]) / n;
		double cur_x, cur_s;
		int spline_index = 1;
		for (int i = 0; i < n; i++) {
			cur_x = x_arr[0] + h * i;
			x_N.push_back(cur_x);
			if (cur_x > x_arr[spline_index]) {
				spline_index++;
			}
			else if (cur_x < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
				spline_index--;
			}
			cur_s = A[spline_index - 1] + B[spline_index - 1] * (cur_x - x_arr[spline_index]) + C[spline_index] / 2 * std::pow((cur_x - x_arr[spline_index]), 2) + D[spline_index - 1] / 6 * std::pow((cur_x - x_arr[spline_index]), 3);
			S.push_back(cur_s);
		}
		return std::pair<std::vector<double>, std::vector<double>> {x_N, S};
	}

	//МЕТОДЫ ДЛЯ ПРОВЕРКИ КОРРЕКТНОСТИ
	void set_func(std::function<double(double)> func)
	{
		this->func = func;
	}

	void research() {
		// подсчёт вспомогательных характеристик
		calc_F();
		calc_dif_F_S();
		calc_dF();
		calc_dS();
		calc_dif_dF_dS();
		calc_d2F();
		calc_d2S();
		calc_dif_d2F_d2S();
	}

};

#include<iostream>
#include<fstream>
extern "C" __declspec(dllexport) void write_to_files(int n, int N, double m1, double m2, double a, double b, int task_num) {
	std::vector<std::function<double(double)>> funcs = { f1, f12 };

	Spline sp(n, a, b, funcs[task_num], m1, m2);
	sp.set_func(funcs[task_num]);
	sp.calculate(N);
	sp.research();
	std::ofstream table_1("table_1.txt");
	std::ofstream table_2("table_2.txt");
	std::ofstream spravka("spravka.txt");
	std::ofstream for_plots("for_plots.txt");
	for (int i = 1; i < sp.n; i++) {
		table_1 << (i) << ' ' << (sp.x_arr[i - 1]) << ' ' << (sp.x_arr[i]) << ' ' << sp.A[i] << ' ' << sp.B[i] << ' ' << sp.C[i] << ' ' << sp.D[i] << '\n';
	}
	for (int j = 0; j < N; j++) {
		table_2 << j << ' ' << sp.x_N[j] << ' ' << sp.F[j] << ' ' << sp.S[j] << ' ' << sp.dif_F_S[j] << ' ' << sp.dF[j] << ' ' << sp.dS[j]<<' ' << sp.dif_dF_dS[j] << '\n';
	}
	spravka << sp.n << ' ' << N << ' ' << sp.max_dif_F_S << ' ' << sp.argmax_dif_F_S << ' ' << sp.max_dif_dF_dS << ' ' << sp.argmax_dif_dF_dS << '\n';
	for (int j = 0; j < N; j++) {
		for_plots << sp.x_N[j]<<' ' << sp.F[j] << ' ' << sp.S[j] << ' ' << sp.dif_F_S[j] << ' ';
		for_plots << sp.dF[j] << ' ' << sp.dS[j] << ' ' << sp.dif_dF_dS[j] << ' ';
		for_plots << sp.d2F[j] << ' ' << sp.d2S[j] << ' ' << sp.dif_d2F_d2S[j] << '\n';
	}
	table_1.close();
	table_2.close();
	spravka.close();
	for_plots.close();
}
