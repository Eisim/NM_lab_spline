import math

columns = {
    "table_1": ["Номер сплайна:\ni",
                "Координата:\nX_(i-1)",
                "Координата:\nX_i",
                "Коэффициент :\na_i",
                "Коэффициент :\nb_i",
                "Коэффициент :\nc_i",
                "Коэффициент :\nd_i",
                ],

    "table_2": ["Номер узла:\nj",
                "Координата:\nX_j",
                "Значение функции в узле:\nF(X_j)",
                "Значение сплайна в узле:\nS(X_j)",
                "Разность:\nF(X_j)-S(X_j)",
                "Производная:\nF'(X_j)",
                "Производная:\nS'(X_j)",
                "Разность производных:\nF'(X_j)-S'(X_j)",
                ],

    # вписать названия колонок
}
extra_info_rows = {
    "spravka": [
        "Сетка сплайна: n =",
        "Контрольная сетка: N =",
        "Погрешность сплайна на контрольной сетке =",
        "при x =",
        "Погрешность производной на контрольной сетке =",
        "при x =",
    ]
    # "out_2": [
    #     "для решения задачи использована равномерная сетка с числом разбиений n =",
    #     "максимальная разность численных решений в общих узлах сетки наблюдается в точке x=",
    #     "задача должна быть решена с заданной точностью ε = 0.5⋅10^–6; задача решена с точностью ε2 ="
    # ]
}

task_standart_params = {
    # <индекс задачи(такой же как в dll)>:<набор стандартных параметров>
    # <набор стандартных параметров> =
    # [
    # a, b,
    # M1, M2
    # ]
    0: [-1, 1, 0, 0],  # тестовая задача
    1: [2, 4, 0, 0],  # f1
    2: [1, 3, 0, 0],
    3: [0, math.pi / 2, 0, 0],
    4: [2, 4, 0, 0],
    5: [1, 3, 0, 0],
    6: [0, math.pi / 2, 0, 0]
}
