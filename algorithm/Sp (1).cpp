// Sp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include<vector>
#include <math.h> // добавляем математические функции
#include <fstream>
#include<iomanip>

std::vector<double> progon(std::vector<std::vector<double>> matr, std::vector<double> b) {
	std::vector<double> res;
	std::vector<double> alfa;
	std::vector<double> betta;
	std::vector<double> temp;
	alfa.push_back((-matr.at(0).at(1)) / matr.at(0).at(0));
	betta.push_back(b.at(0) / matr.at(0).at(0));
	int i = 1;
	double x = 0;
	while (i < b.size() - 1) {
		x = matr.at(i).at(1) + matr.at(i).at(0) * alfa.at(i - 1);
		alfa.push_back(-matr.at(i).at(2) / x);
		betta.push_back((b.at(i) - matr.at(i).at(0) * betta.at(i - 1)) / x);
		i++;
	}
	x = matr.at(i).at(1) + matr.at(i).at(0) * alfa.at(i - 1);
	betta.push_back((b.at(i) - matr.at(i).at(0) * betta.at(i - 1)) / x);

	temp.push_back(betta.at(i));
	i--;
	while (i > -1) {
		temp.push_back(alfa.at(i) * temp.back() + betta.at(i));
		i--;
	}
	int j = temp.size();
	res.resize(j);

	std::vector<double>::iterator Iter = res.begin();
	while (j != 0) {
		*Iter = (temp.at(j - 1));
		j--;
		Iter++;
	}
	return res;
}

double f1(double x) {
	return (pow(x, 3) + 3 * pow(x, 2));
}

double f2(double x) {
	return (-pow(x, 3) + 3 * pow(x, 2));
}
double f12(double x) {
	return sin(x) / x;
}

class Spline {


private:

	double S(double x, int i) { // x -аргумент i - номер S1 S2 S3 и тд // канон вид S= ai + bi(x-xi) + ci/2 (x-xi)^2 + di/6(x-xi)^3
		return A[i - 1] + B[i - 1] * (x - X_i[i]) + C[i] / 2 * pow((x - X_i[i]), 2) + D[i - 1] / 6 * pow((x - X_i[i]), 3);

	}
	double derivative_1(double f(double), double x, double h) { // первая производная f' = (f(x+h) - f(x-h))/2h
		return (f(x + h) - f(x - h)) / (2 * h);
	}
	double derivative_1(double x, int i) { // первая производная для сплайна bi*x + ci *(x-xi) + di/2(x-xi)^2
		return +B[i - 1] + C[i] * (x - X_i[i]) + D[i - 1] / 2 * pow((x - X_i[i]), 2);
	}
	double derivative_2(double f(double), double x, double h) { // вторая производная f'' = (f(x+h) -2f(x) + f(x-h))/h^2
		return (f(x + h) - 2 * f(x) + f(x - h)) / pow(h, 2);
	}
	double derivative_2(double x, int i) { // вторая производная для сплайна ci *x + di(x - xi)
		return C[i] + D[i - 1] * (x - X_i[i]);
	}

	void find_C(double f(double), double g(double)) { //  find_P : P ={A,B,C,D} поиск соответственных коэффециэнтов , формулы можно в методичке по сплайнам найти (Для условий S''(a) =M1 , S''(b) =M2 !!!)
		if (n == 2) {
			C.push_back(M1);
			C.push_back((6 / h * ((F_i[2] - F_i[1]) - (F_i[1] - F_i[0])) - (M1 * h) - (M2 * h)) / (4 * h));
			C.push_back(M2);
			find_A();
			find_B();
			find_D();
			if (f == g) {
				examination(f);
			}
			else {
				examination(f, g);
			}
		}
		else {
			int m = n - 1;
			std::vector<std::vector <double>> matr; // n-1 x n-1
			std::vector<double> str; // левая часть , строка n - 1
			std::vector<double> rhs; // правая часть n - 1
			rhs.push_back(6 / h * ((F_i[2] - F_i[1]) - (F_i[1] - F_i[0])) - (M1 * h)); // первая строка матрицы
			str.push_back(4 * h);
			str.push_back(h);
			matr.push_back(str);
			/*
			если шаг константа то матрица будет иметь след вид:
			4h h 0 ... 0         |то что там с ф - M1 * h
			h 4h h ... 0		 |
			0 h 4h h ... 0		 | то что там с Ф
			...               =  |
			...					 |
			0 ... h 4h           | то что там с Ф - M2 * h
			для прогонки мне надо передать не нулевые коэффециэнты
			*/

			for (int i = 2; i <= m - 1; i++) { // подсчет строк со 2 по m-2
				str.clear();
				str.push_back(h);
				str.push_back(4 * h);
				str.push_back(h);
				matr.push_back(str);
				rhs.push_back(6 / h * ((F_i[i + 1] - F_i[i]) - (F_i[i] - F_i[i - 1])));
			}
			str.clear();
			rhs.push_back(6 / h * ((F_i[m + 1] - F_i[m]) - (F_i[m] - F_i[m - 1])) - (M2 * h)); // подсчет последний строки
			str.push_back(h);
			str.push_back(4 * h);
			matr.push_back(str);
			C = progon(matr, rhs);
			auto iter = C.cbegin();
			C.emplace(iter, M1);
			C.push_back(M2);
			find_A();
			find_B();
			find_D();
			if (f == g) {
				examination(f);
			}
			else {
				examination(f, g);
			}
		}
	}

	void find_A() {
		for (int i = 1; i <= n; i++) A.push_back(F_i[i]);
	}
	void find_D() {
		for (int i = 1; i <= n; i++) D.push_back((C[i] - C[i - 1]) / h);
	}
	void find_B() {
		for (int i = 1; i <= n; i++) {
			B.push_back((F_i[i] - F_i[i - 1]) / h + C[i] * h / 3 + C[i - 1] * h / 6);
		}
	}
	void clear() {
		C.clear();
		A.clear();
		B.clear();
		D.clear();
		X_i.clear();
		X_2_i.clear();
		F_i.clear();
		r_1.clear();
		r_2.clear();
		r_3.clear();

	}
	void examination(double f(double)) { // удваивает начальное n пока не добьеться нужной точности   ///в производной шаг константный у меня , но если надо можете туда что то другое передавать
		double x = 0;
		double X_max;
		int j = 1;
		double max_1 = fabs(f(a) - S(a, 1));
		double max_2 = fabs(derivative_1(f, a, 0.001) - derivative_1(a, 1));
		double max_3 = fabs(derivative_2(f, a, 0.001) - derivative_2(a, 1));
		for (int i = 0; i <= 2 * n; i++) {
			x = a + h / 2 * i;
			X_2_i.push_back(x);
			if (x <= X_i[j]) {
				if (fabs(f(x) - S(x, j)) > max_1) {
					max_1 = fabs(f(x) - S(x, j));
					X_max = x;
				}
				if ((fabs(derivative_1(f, x, 0.001) - derivative_1(x, j))) > max_2) max_2 = fabs(derivative_1(f, x, 0.001) - derivative_1(x, j));
				if ((fabs(derivative_2(f, x, 0.001) - derivative_2(x, j))) > max_3) max_3 = fabs(derivative_2(f, x, 0.001) - derivative_2(x, j));
			}

			else {
				j++;
				if (fabs(f(x) - S(x, j)) > max_1) {
					max_1 = fabs(f(x) - S(x, j));
					X_max = x;
				}
				if ((fabs(derivative_1(f, x, 0.001) - derivative_1(x, j))) > max_2) max_2 = fabs(derivative_1(f, x, 0.001) - derivative_1(x, j));
				if ((fabs(derivative_2(f, x, 0.001) - derivative_2(x, j))) > max_3) max_3 = fabs(derivative_2(f, x, 0.001) - derivative_2(x, j));
			}
			r_1.push_back(fabs(f(x) - S(x, j)));
			r_2.push_back(fabs(derivative_1(f, x, 0.001) - derivative_1(x, j)));
			r_3.push_back(fabs(derivative_2(f, x, 0.001) - derivative_2(x, j)));

		}


		max_eps = max_1;
		if ((max_eps - 1e-06) > 0) {
			this->n = n * 2;
			clear();
			this->h = abs(a - b) / n;
			A_12(f12);
		}
		else {
			std::cout << " n =  " << n << std::endl; // это все теперь в файл фигануть надо, но предлагаю сначало консольно сделать.
			std::cout << " max1(F(x) - S(x)) =  " << max_1 << std::endl;
			std::cout << " max2(F'(x) - S'(x)) =  " << max_2 << std::endl;
			std::cout << " max3(F''(x) - S''(x)) =  " << max_3 << std::endl;
		}
	}

	void examination(double f(double), double g(double)) { // проверяет корректность тестовой задачи (A)
		double x = 0;
		int j = 1;
		double max_1 = fabs(f(a) - S(a, 1));
		double max_2 = fabs(derivative_1(f, a, 0.01) - derivative_1(a, 1));
		double max_3 = fabs(derivative_2(f, a, 0.01) - derivative_2(a, 1));
		for (int i = 0; i <= 2 * n; i++) {
			x = -1 + h / 2 * i;
			X_2_i.push_back(x);
			if (x <= X_i[j]) {
				if (x <= 0) {
					if (fabs(f(x) - S(x, j)) > max_1) max_1 = fabs(f(x) - S(x, j));
					if (fabs(derivative_1(f, x, 0.01) - derivative_1(x, j))) max_2 = fabs(derivative_1(f, x, 0.01) - derivative_1(x, j));
					if (fabs(derivative_2(f, x, 0.01) - derivative_2(x, j))) max_3 = fabs(derivative_2(f, x, 0.01) - derivative_2(x, j));
					r_1.push_back(fabs(f(x) - S(x, j)));
					r_2.push_back(fabs(derivative_1(f, x, 0.001) - derivative_1(x, j)));
					r_3.push_back(fabs(derivative_2(f, x, 0.001) - derivative_2(x, j)));
				}
				else {
					if (fabs(g(x) - S(x, j)) > max_1) max_1 = fabs(g(x) - S(x, j));
					if (fabs(derivative_1(g, x, 0.01) - derivative_1(x, j))) max_2 = fabs(derivative_1(g, x, 0.01) - derivative_1(x, j));
					if (fabs(derivative_2(g, x, 0.01) - derivative_2(x, j))) max_3 = fabs(derivative_2(g, x, 0.01) - derivative_2(x, j));
					r_1.push_back(fabs(g(x) - S(x, j)));
					r_2.push_back(fabs(derivative_1(g, x, 0.001) - derivative_1(x, j)));
					r_3.push_back(fabs(derivative_2(g, x, 0.001) - derivative_2(x, j)));
				}
			}
			else {
				j++;
				if (x <= 0) {
					if (fabs(f(x) - S(x, j)) > max_1) max_1 = fabs(f(x) - S(x, j));
					if (fabs(derivative_1(f, x, 0.01) - derivative_1(x, j))) max_2 = fabs(derivative_1(f, x, 0.01) - derivative_1(x, j));
					if (fabs(derivative_2(f, x, 0.01) - derivative_2(x, j))) max_3 = fabs(derivative_2(f, x, 0.01) - derivative_2(x, j));
					r_1.push_back(fabs(f(x) - S(x, j)));
					r_2.push_back(fabs(derivative_1(f, x, 0.001) - derivative_1(x, j)));
					r_3.push_back(fabs(derivative_2(f, x, 0.001) - derivative_2(x, j)));
				}
				else {
					if (fabs(g(x) - S(x, j)) > max_1) max_1 = fabs(g(x) - S(x, j));
					if (fabs(derivative_1(g, x, 0.01) - derivative_1(x, j))) max_2 = fabs(derivative_1(g, x, 0.01) - derivative_1(x, j));
					if (fabs(derivative_2(g, x, 0.01) - derivative_2(x, j))) max_3 = fabs(derivative_2(g, x, 0.01) - derivative_2(x, j));
					r_1.push_back(fabs(g(x) - S(x, j)));
					r_2.push_back(fabs(derivative_1(g, x, 0.001) - derivative_1(x, j)));
					r_3.push_back(fabs(derivative_2(g, x, 0.001) - derivative_2(x, j)));
				}
			}
		}
		// это все теперь в файл фигануть надо, но предлагаю сначало консольно сделать.
		std::cout << " max1(F(x) - S(x)) =  " << max_1 << std::endl;
		std::cout << " max2(F'(x) - S'(x)) =  " << max_2 << std::endl;
		std::cout << " max3(F''(x) - S''(x)) =  " << max_3 << std::endl;

	}
	void test_func(double f(double), double g(double)) { // пункт а) тестовая задача.
		double x = 0;
		for (int i = 0; i <= n; i++) {
			x = -1 + h * i;
			X_i.push_back(x);
			if (x <= 0) {
				F_i.push_back(f(x));
			}
			else {
				F_i.push_back(g(x));
			}
		}
		//for (int i = 0; i <= n; i++) {
			//std::cout << "X_i : " << X_i[i] << " F_i : " << F_i[i] << std::endl;
		//}
		find_C(f, g);

		return;
	}
	void A_12(double f(double)) {
		double x = 0;
		for (int i = 0; i <= n; i++) {
			x = a + h * i;
			X_i.push_back(x);
			F_i.push_back(f(x));

		}
		find_C(f, f);
	}
public:
	// это все теперь в файл фигануть надо, но предлагаю сначало консольно сделать.
	std::vector<double> X_i; // узлы с шагом h
	std::vector<double> F_i; // значения функции в узлах 
	std::vector<double> C; // коэф-ты С
	std::vector<double> A;// коэф-ты А
	std::vector<double> B; // коэф-ты В
	std::vector<double> D; // коэф-ты D
	std::vector<double> X_2_i; // узлы с шагом h/2
	std::vector<double> r_1; // F - S
	std::vector<double> r_2; // F' - S'
	std::vector<double> r_3; // F'' - S''
	double M1 = 0;// S''(a)
	double M2 = 0;// S''(b)
	double h, a, b; // Шаг, левая граница, правая граница
	int n; // число разбиений  -> число узлов n+1
	double max_eps; // точность 

	Spline(int n, double M1, double M2, double a, double b) { // проверку на корректность n  не делаю, это в интерфейсе должно быть.
		this->n = n;
		this->M1 = M1;
		this->M2 = M2;
		this->a = a;
		this->b = b;
		this->h = abs(a - b) / n;
	}
	void Run(int value) {
		switch (value)
		{
		case 1:
			test_func(f1, f2);
			break;
		case 2:
			A_12(f12);
			break;
		default:
			break;
		}
	}
};

extern "C" __declspec(dllexport) void f_test(int n, double m1, double m2, double a, double b) {
	Spline sp(n, m1, m2, a, b);
	sp.Run(1);
	std::ofstream table_1("data/table_1.txt");
	std::ofstream table_2("data/table_2.txt");
	std::ofstream spravka("data/spravka.txt");
	std::ofstream for_plots("data/for_plots.txt");
	for (int i = 1; i < sp.n; i++) {
		table_1 << (i) << ' ' << (sp.X_i[i - 1]) << ' ' << (sp.X_i[i]) << ' ' << sp.A[i] << ' ' << sp.B[i] << ' ' << sp.C[i] << ' ' << sp.D[i] << '\n';
	}
	table_1.close();
}




int main()
{
	f_test(10, 0, 0, 0, 1);
}


