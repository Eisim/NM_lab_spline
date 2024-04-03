#include<vector>
#include<functional>
#include<cmath>


double f_test(double x) {
	if (x >= -1 && x <= 0)
		return (std::pow(x, 3) + 3 * std::pow(x, 2));
	else if (x >= 0 && x <= 1)
		return (-std::pow(x, 3) + 3 * std::pow(x, 2));
	throw std::exception("function is not defined");
}
double df_test(double x) {
	if (x>=-1 && x<=0) {
		return 3 * x * x + 6 * x;
	}
	else if (x >= 0 && x <= 1) {
		return -3 * x * x + 6 * x;
	}
	throw std::exception("function is not defined");
}
double d2f_test(double x) {
	if (x >= -1 && x <= 0) {
		return 6 * x + 6;
	}
	else if (x >= 0 && x <= 1) {
		return -6 * x + 6;
	}
	throw std::exception("function is not defined");
}

/*
//функция из exe файла
double one_more_test_func(double x) {
	return 1 / (1 + x * x * x);
}
double one_more_test_dfunc(double x) {
	return -(3*x*x) / (std::pow(x,6)+2*std::pow(x,3) + 1);
}
double one_more_test_d2func(double x) {
	return (12*std::pow(x,4)-6*x)/(std::pow(x,9)+3*std::pow(x,6)+3*std::pow(x,3)+1);
}
*/

//Если я взял не те варианты, поменяйте :-)
//test func

//func 1
double f1(double x) {
	return std::sqrt(x * x - 1) / x;
}
double df1(double x) {
	return 1 / (x * x * std::sqrt(x * x - 1));
}
double d2f1(double x) {
	return -( std::sqrt(x*x-1)*(3*x*x-2) ) / (std::pow(x,7)-2*std::pow(x,5) + std::pow(x,3) );
}

//func 12
double f12(double x) {
	return std::sin(x) / x;
}
double df12(double x) {
	return -(std::sin(x) - x*std::cos(x)) / (x * x);
}
double d2f12(double x) {
	return -((x*x-2)*std::sin(x) + 2*x*std::cos(x)) / (x * x * x);
}

//func 23
double f23(double x) {
	return std::sqrt(4-2*std::sin(x));
}
double df23(double x) {
	return -(std::cos(x)) / ( std::sqrt(4-2*std::sin(x)) );
}
double d2f23(double x) {
	return -(std::sqrt(4 - 2 * std::sin(x)) * (2 * std::pow(std::sin(x), 2) - 4 * std::sin(x) + std::pow(std::cos(x),2))) / (4 * std::pow(std::sin(x), 2) - 16 * std::sin(x) + 16);
}
//сделать для осциллирующей функции
//сделать для осциллирующей функции
//сделать для осциллирующей функции

//F1
double F1_oscillating(double x) {
	return f1(x)+std::cos(10*x);
}
double dF1_oscillating(double x) {
	return df1(x)-10*std::sin(10*x);
}
double d2F1_oscillating(double x) {
	return d2f1(x)-100*std::cos(10*x);
}
//F12
double F12_oscillating(double x) {
	return f12(x) + std::cos(10 * x);
}
double dF12_oscillating(double x) {
	return df12(x) - 10 * std::sin(10 * x);
}
double d2F12_oscillating(double x) {
	return d2f12(x) - 100 * std::cos(10 * x);
}
//F23
double F23_oscillating(double x) {
	return f23(x) + std::cos(10 * x);
}
double dF23_oscillating(double x) {
	return df23(x) - 10 * std::sin(10 * x);
}
double d2F23_oscillating(double x) {
	return d2f23(x) - 100 * std::cos(10 * x);
}



/*
std::vector<std::function<double(double)>> funcs = { one_more_test_func,f1, f12,f23, F1_oscillating,F2_oscillating,F3_oscillating };
std::vector<std::function<double(double)>> d_funcs = { one_more_test_dfunc,df1, df12,df23, dF1_oscillating ,dF2_oscillating ,dF3_oscillating };
std::vector<std::function<double(double)>> d2_funcs = { one_more_test_d2func,d2f1, d2f12,d2f23,d2F1_oscillating, d2F2_oscillating, d2F3_oscillating };
*/
std::vector<std::function<double(double)>> funcs = { f_test,f1, f12,f23, F1_oscillating,F12_oscillating,F23_oscillating };
std::vector<std::function<double(double)>> d_funcs = { df_test,df1, df12,df23, dF1_oscillating ,dF12_oscillating ,dF23_oscillating };
std::vector<std::function<double(double)>> d2_funcs = { d2f_test,d2f1, d2f12,d2f23,d2F1_oscillating, d2F12_oscillating, d2F23_oscillating };




class Spline {
private:

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
		for (auto e: x_N) {
			dF.push_back(d_func(e));
		}
	}
	void calc_dS() {
		int spline_index = 1;
		double cur_ds;
		for (auto e : x_N) {
			if (e > x_arr[spline_index]) {
				spline_index++;
			}
			else if (e < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
				spline_index--;
			}
			cur_ds = B[spline_index - 1] + C[spline_index] * (e - x_arr[spline_index]) + D[spline_index - 1] / 2 * std::pow((e - x_arr[spline_index]), 2);
			dS.push_back(cur_ds);
		}
	}
	void calc_d2F() { // вторая производная f'' = (f(x+h) -2f(x) + f(x-h))/h^2
		double h = 0.001;
		for (auto e : x_N)
			d2F.push_back(d2_func(e));
	}
	void calc_d2S() { // вторая производная для сплайна ci + di(x - xi)
		int spline_index = 1;
		{
			int spline_index = 1;
			double cur_d2s;
			for (auto e : x_N) {
				if (e > x_arr[spline_index]) {
					spline_index++;
				}
				else if (e < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
					spline_index--;
				}
				cur_d2s =C[spline_index]  + D[spline_index - 1]* (e - x_arr[spline_index]);
				d2S.push_back(cur_d2s);
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
	void calc_max_F_S() {
		max_dif_F_S = -1.;
		for (int i = 0; i < dif_F_S.size(); i++) {
			if (max_dif_F_S < dif_F_S[i]) {
				max_dif_F_S = dif_F_S[i];
				argmax_dif_F_S = x_N[i];

			}
		}
	}
	void calc_max_dF_dS() {
		max_dif_dF_dS = -1.;
		for (int i = 0; i < dif_dF_dS.size(); i++) {
			if (max_dif_dF_dS < dif_dF_dS[i]) {
				max_dif_dF_dS = dif_dF_dS[i];
				argmax_dif_dF_dS = x_N[i];
			}
		}
	}
public:
	std::function<double(double)> func, d_func, d2_func;

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
	std::vector<double> calc_dS(std::vector<double> x) {
		int spline_index = 1;
		double cur_ds;
		std::vector<double> tmp_dS;
		for (auto e : x) {
			if (e > x_arr[spline_index]) {
				spline_index++;
			}
			else if (e < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
				spline_index--;
			}
			cur_ds = B[spline_index - 1] + C[spline_index] * (e - x_arr[spline_index]) + D[spline_index - 1] / 2 * std::pow((e - x_arr[spline_index]), 2);
			tmp_dS.push_back(cur_ds);
		}
		return tmp_dS;
	}

	// задание функции(делается для тестов) 
	// ЗАМЕЧАНИЕ: Spline не будет сохранять функцию, если она передаётся в какой-либо другой метод класса или конструктор
	std::pair<std::vector<double>, std::vector<double>> calculate(int n,bool inplace = true) {//n - число разбиений
		std::vector<double> *tmp_X, *tmp_S;
		if (inplace) {
			tmp_X = &x_N;
			tmp_S = &S;
		}
		else {
			tmp_X = new std::vector<double>;
			tmp_S = new std::vector<double>;
		}

		double h = (x_arr[x_arr.size() - 1] - x_arr[0]) / n;
		double cur_x, cur_s;
		int spline_index = 1;
		for (int i = 0; i <= n; i++) {
			cur_x = x_arr[0] + h * i;
			(*tmp_X).push_back(cur_x);
			if (cur_x > x_arr[spline_index]) {
				spline_index++;
			}
			else if (cur_x < x_arr[spline_index - 1]) {//по идее этого условия не должно быть
				spline_index--;
			}
			cur_s = A[spline_index - 1] + B[spline_index - 1] * (cur_x - x_arr[spline_index]) + C[spline_index] / 2 * std::pow((cur_x - x_arr[spline_index]), 2) + D[spline_index - 1] / 6 * std::pow((cur_x - x_arr[spline_index]), 3);
			(*tmp_S).push_back(cur_s);
		}
		std::pair<std::vector<double>, std::vector<double>> res = {*tmp_X, *tmp_S};
		if (!inplace) {
			delete tmp_X;
			delete tmp_S;
		}
		return res;
	}

	//МЕТОДЫ ДЛЯ ПРОВЕРКИ КОРРЕКТНОСТИ
	
	void research(int task_num, int N) {
		// подсчёт вспомогательных характеристик
		this->func = funcs[task_num];
		this->d_func = d_funcs[task_num];
		this->d2_func = d2_funcs[task_num];

		calculate(N);
		calc_F();
		calc_dif_F_S();
		calc_dF();
		calc_dS();
		calc_dif_dF_dS();
		calc_d2F();
		calc_d2S();
		calc_dif_d2F_d2S();

		calc_max_F_S();
		calc_max_dF_dS();
	}

};



#include<iostream>
#include<fstream>
extern "C" __declspec(dllexport) void write_to_files(int n, int N, double m1, double m2, double a, double b,int task_type, int task_num) {
	int task_index = 0;
	if (task_type == 0)
		task_index = 0;
	else if(task_type ==1) {
		task_index = task_num + 1;
	}
	else if (task_type == 2) {
		task_index = task_num + 4;
	}
	Spline sp(n, a, b, funcs[task_index], m1, m2);
	sp.research(task_index,N);
	std::ofstream table_1("table_1.txt");
	std::ofstream table_2("table_2.txt");
	std::ofstream spravka("spravka.txt");
	std::ofstream for_plots("for_plots.txt");
	std::ofstream plots_dots("plots_dots.txt");
	for (int i = 1; i <= sp.n; i++) {
		table_1 << (i) << ' ' << (sp.x_arr[i - 1]) << ' ' << (sp.x_arr[i]) << ' ' << sp.A[i-1] << ' ' << sp.B[i-1] << ' ' << sp.C[i] << ' ' << sp.D[i-1] << '\n';
	}
	for (int j = 0; j <= N; j++) {
		table_2 << j << ' ' << sp.x_N[j] << ' ' << sp.F[j] << ' ' << sp.S[j] << ' ' << sp.dif_F_S[j] << ' ' << sp.dF[j] << ' ' << sp.dS[j]<<' ' << sp.dif_dF_dS[j] << '\n';
	}
	spravka << sp.n << ' ' << N << ' ' << sp.max_dif_F_S << ' ' << sp.argmax_dif_F_S << ' ' << sp.max_dif_dF_dS << ' ' << sp.argmax_dif_dF_dS << '\n';

	for (int j = 0; j <= N; j++) {
		for_plots << sp.x_N[j]<<' ' << sp.F[j] << ' ' << sp.S[j] << ' ' << sp.dif_F_S[j] << ' ';
		for_plots << sp.dF[j] << ' ' << sp.dS[j] << ' ' << sp.dif_dF_dS[j] << ' ';
		for_plots << sp.d2F[j] << ' ' << sp.d2S[j] << ' ' << sp.dif_d2F_d2S[j] << '\n';
	}
	int x_arr_for_plots = 5000; // Какое-то количество точек по которым будет строиться график
	std::pair<std::vector<double>,std::vector<double>> plots_dots_arr = sp.calculate(x_arr_for_plots,false);
	std::vector<double> tmp_dS = sp.calc_dS(plots_dots_arr.first);
	for (int j = 0; j <= x_arr_for_plots;j++) {
		plots_dots << plots_dots_arr.first[j] << ' ' << plots_dots_arr.second[j] << ' ' << sp.func(plots_dots_arr.first[j])<<' '<< tmp_dS[j]<<' '<<sp.d_func(plots_dots_arr.first[j])<< '\n';
	}

	table_1.close();
	table_2.close();
	spravka.close();
	for_plots.close();
	plots_dots.close();
}
