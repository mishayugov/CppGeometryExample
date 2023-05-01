#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <fstream>

#include <bits/stdc++.h>

// первое множество
static const int SET_1 = 0;
// второе множество
static const int SET_2 = 1;
// пересечение множеств
static const int SET_CROSSED = 2;
// разность множеств
static const int SET_SINGLE = 3;

// Ширина окна
static const int WINDOW_SIZE_X = 800;
// Высота окна
static const int WINDOW_SIZE_Y = 800;

// путь к файлу вывода
static const char OUTPUT_PATH[255] = "C:/Users/dimay/CLionProjects/out.txt";
// путь к файлу ввода
static const char INPUT_PATH[255] = "C:/Users/dimay/CLionProjects/in.txt";

double pi=3.14159265;

// точка
struct Point {
    // положение
    sf::Vector2i pos;

    // конструктор
    Point(const sf::Vector2i &pos) : pos(pos) {
    }
    // получить случайную точку
    static Point randomPoint() {
        return Point(sf::Vector2i(
                             rand() % WINDOW_SIZE_X,
                             rand() % WINDOW_SIZE_Y)
        );
    }
};

// динамический список точек для Монте-Карло
std::vector<Point> MKpoints;
// добавить много случайных точек для Монте-Карло
void randomize() {
    MKpoints.clear();
    for (long long i = 0; i < 100000; i++) {
        MKpoints.emplace_back(Point::randomPoint());
    }
}

// круг
struct Circle {
    // положение центра
    sf::Vector2i pos;
    // положение точки на окружности
    sf::Vector2i pos2;

    // конструктор
    Circle(const sf::Vector2i &pos, const sf::Vector2i &pos2) : pos(pos), pos2(pos2) {
    }

    //случайная окружность
    static Circle randomCircle() {
        return Circle (sf::Vector2i(rand()%WINDOW_SIZE_X,rand()%WINDOW_SIZE_Y),sf::Vector2i(rand()%WINDOW_SIZE_X,rand()%WINDOW_SIZE_Y));
    }
};

// динамический список кругов
std::vector<Circle> circles;

// угол
struct Angle {
    // положение центра
    sf::Vector2i pos1;
    // положение твух точек
    sf::Vector2i pos2;
    sf::Vector2i pos3;

    // конструктор
    Angle(const sf::Vector2i &pos1, const sf::Vector2i &pos2,const sf::Vector2i &pos3) : pos1(pos1), pos2(pos2),pos3(pos3) {
    }

    //случайный угол
    static Angle randomAngle() {
        return Angle (sf::Vector2i(rand()%WINDOW_SIZE_X,rand()%WINDOW_SIZE_Y),sf::Vector2i(rand()%WINDOW_SIZE_X,rand()%WINDOW_SIZE_Y),sf::Vector2i(rand()%WINDOW_SIZE_X,rand()%WINDOW_SIZE_Y));
    }
};

// динамический список углов
std::vector<Angle> angles;


// цвет фона
static sf::Color bgColor;
// значение цвета по умолчанию
float color[3] = {0.12f, 0.12f, 0.13f};

// буфер, хранящий координаты последней добавленной точки правой кнопкой
int fAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпоследней добавленной точки правой кнопкой
int sAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки правой кнопкой
int tAddPosBufR[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки левой кнопкой
int fAddPosBufL[2] = {-1, 0};
// буфер, хранящий координаты предпредпоследней добавленной точки левой кнопкой
int sAddPosBufL[2] = {-1, 0};


// задать цвет фона по вещественному массиву компонент
static void setColor(float *pDouble) {
    bgColor.r = static_cast<sf::Uint8>(pDouble[0] * 255.f);
    bgColor.g = static_cast<sf::Uint8>(pDouble[1] * 255.f);
    bgColor.b = static_cast<sf::Uint8>(pDouble[2] * 255.f);
}



// запись в файл
void saveToFile() {
    // открываем поток данных для записи в файл
    std::ofstream output(OUTPUT_PATH);

    // перебираем окружности
    for (auto circle: circles) {
        output << 0<<" "<<circle.pos.x << " " << circle.pos.y << " " << circle.pos2.x <<" "<<circle.pos2.y <<std::endl;
    }
    // перебираем углы
    for (auto angle: angles) {
        output << 1<<" "<<angle.pos1.x << " " << angle.pos1.y << " " << angle.pos2.x << " " << angle.pos2.y<< " "<< angle.pos3.x << " " << angle.pos3.y << std::endl;
    }
    // закрываем
    output.close();
}

//пересекаются ли два отрезка?
bool tagIntersect(sf:: Vector2i x1y1, sf:: Vector2i x2y2, sf:: Vector2i a1b1, sf:: Vector2i a2b2) {
    double x1=x1y1.x;
    double y1=x1y1.y;
    double x2=x2y2.x;
    double y2=x2y2.y;
    double x3=a1b1.x;
    double y3=a1b1.y;
    double x4=a2b2.x;
    double y4=a2b2.y;
    double x=((x1*y2-x2*y1)*(x3-x4)-(x3*y4-x4*y3)*(x1-x2))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
    if (((x1<=x and x2>=x) or (x1>=x and x2<=x)) and ((x3<=x and x4>=x) or (x3>=x and x4<=x)))
        return true;
    else
        return false;
}
//функция, определяющая, лежит ли точка внутри угла
bool point_in_angle(Angle a, Point p){
    sf:: Vector2i sm1,sm2,smp,sm0={0,0};
    sm1.x=(a.pos2.x-a.pos1.x);
    sm1.y=(a.pos2.y-a.pos1.y) ;
    sm2.x=(a.pos3.x-a.pos1.x);
    sm2.y=(a.pos3.y-a.pos1.y);
    smp.x=(p.pos.x-a.pos1.x) *100;
    smp.y=(p.pos.y-a.pos1.y) *100;
    return tagIntersect(sm0,smp,sm1,sm2);
}
//лежит ли точка внутри круга
bool point_in_circle(Circle a, Point p){
    double rad=(a.pos.x-a.pos2.x)*(a.pos.x-a.pos2.x)+(a.pos.y-a.pos2.y)*(a.pos.y-a.pos2.y);
    if (rad>=(a.pos.x-p.pos.x)*(a.pos.x-p.pos.x)+(a.pos.y-p.pos.y)*(a.pos.y-p.pos.y)){
        return true;
    } else {
        return false;
    }
}

//здесь лежат индексы в массиве окружности и угла, которые нужно рисовать
long long ind_c,ind_a,nado_li=0;

//множество точек, по которым надо построить контур пересечения и его построение
//самая интеллектуальная часть программы, на мой взгляд
std::vector<Point> P_circuit;

//делаю множество точек на единичной окружности
std::vector<std::pair<double, double>> Set;
void making_a_set(Angle A,Circle C,double rad_C){
    Set.clear();
    double cos1=cos(2*pi/1000),sin1=sin(2*pi/1000);
    std::pair<double,double> pp;
    pp.first=cos1;
    pp.second=sin1;
    Set.push_back(pp);
    for (long int i=0;i<999;i++){
        cos1=cos(2*pi/1000*(i+1));
        sin1=sin(2*pi/1000*(i+1));
        if (abs(cos1*rad_C-(A.pos1.x-C.pos.x))<1)
            continue;
        pp.first=cos1;
        pp.second=sin1;
        Set.emplace_back(pp);
    }
}
void circuit(){
    P_circuit.clear();
    Circle C = circles[ind_c];
    double rad_C = sqrt(pow(C.pos.x-C.pos2.x,2)+pow(C.pos.y-C.pos2.y,2));
    Angle A = angles[ind_a];
    making_a_set(A,C,rad_C);
    std::vector<std::pair<double, double>> Set2;
    std::pair<double,double> u1;
    bool b=false;
    if (point_in_circle(C,A.pos1)){
        u1.first=A.pos1.x;
        u1.second=A.pos1.y;
        b=true;
    }
    for (long int i=0;i<Set.size();i++){
        std::pair<double,double> u;
        u=Set[i];
        u.first*=rad_C;
        u.second*=rad_C;
        u.first+=C.pos.x;
        u.second+=C.pos.y;
        Set2.push_back(u);
    }
    std::vector<std::pair<double, double>> Set3;
    for (long int i=0;i<Set2.size();i++){
        Point pp=MKpoints[0],pp1=MKpoints[0];
        pp.pos.x=Set2[i].first;
        pp.pos.y=Set2[i].second;
        pp1.pos.x=Set2[(i+1)%Set2.size()].first;
        pp1.pos.y=Set2[(i+1)%Set2.size()].second;
        if (point_in_angle(A,pp)){
            Set3.push_back(Set2[i]);
            if (not point_in_angle(A,pp1) and b){
                Set3.push_back(u1);
            }
        }
    }
    for (long int i=0;i<Set3.size();i++){
        Point pp=MKpoints[0];
        pp.pos.x=Set3[i].first;
        pp.pos.y=Set3[i].second;
        P_circuit.emplace_back(pp);
    }
}

// решение задачи
void solve() {
    //генерирую много случайных точек
    randomize();
    //перебираю для каждой пары окружности и угла сколько внутри точек, ищу максимум
    long long maxkol=0;
    ind_c=0;
    ind_a=0;
    for (long int i1=0;i1<circles.size();i1++) {
        for (long int i2 = 0; i2 < angles.size(); i2++) {
            long long kol = 0;
            for (long int i3 = 0; i3 < MKpoints.size(); i3++) {
                if (point_in_angle(angles[i2], MKpoints[i3]) and point_in_circle(circles[i1], MKpoints[i3]))
                    kol++;
            }
            if (kol > maxkol) {
                maxkol = kol;
                ind_c = i1;
                ind_a = i2;
            }
        }
    }
    nado_li=1;
    if (maxkol==0)
        nado_li=0;
    else
        circuit();
}

// загрузка из файла
void loadFromFile() {
    // открываем поток данных для чтения из файла
    std::ifstream input(INPUT_PATH);
    // очищаем массив точек
    circles.clear();
    angles.clear();
    // пока не достигнут конец файла
    while (!input.eof()) {
        int x, y, rad, L,x1,x2,x3,y1,y2,y3;
        input >> L;
        if (L == 0) {
            input >> x; // читаем x координату центра
            input >> y; // читаем y координату центра
            input >> x1; // читаем x координату точки на окружности
            input>>y1; // читаем y координату точки на окружности
            // добавляем в динамический массив окружность на основе прочитанных данных
            circles.emplace_back(Circle(sf::Vector2i(x, y), sf::Vector2i(x1, y1)));
        }
        if (L != 0) {
            input >> x1;
            input >> y1;
            input >> x2;
            input >> y2;
            input >> x3;
            input >> y3;
            // добавляем в динамический массив угол на основе прочитанных данных
            angles.emplace_back(Angle(sf::Vector2i(x1, y1), sf::Vector2i(x2, y2), sf::Vector2i(x3, y3)));
        }
    }
    // закрываем файл
    input.close();
}

// рисование параметров цвета
void ShowBackgroundSetting() {
    // если не раскрыта панель `Background`
    if (!ImGui::CollapsingHeader("Background"))
        // заканчиваем выполнение
        return;

    // Инструмент выбора цвета
    if (ImGui::ColorEdit3("Background color", color)) {
        // код вызывается при изменении значения
        // задаём цвет фона
        setColor(color);
    }
}

// сумма векторов
static sf::Vector2i sum(sf::Vector2i a, sf::Vector2i b) {
    return {a.x + b.x, a.y + b.y};
}

// разность векторов
static sf::Vector2i subtract(sf::Vector2i a, sf::Vector2i b) {
    return {a.x - b.x, a.y - b.y};
}

// умножение вектора на число
static sf::Vector2f mul(sf::Vector2f a, float b) {
    return {a.x * b, a.y * b};
}

// преобразование целочисленного вектора в вещественный
static sf::Vector2f toFloat(sf::Vector2i v) {
    return {(float)v.x, (float)v.y};
}

// преобразование вещественного вектора в целочисленный
static sf::Vector2i toInt(sf::Vector2f v) {
    return {(int) v.x, (int) v.y};
}

// рисование задачи на невидимом окне во всё окно приложения
void RenderTask() {
    // задаём левый верхний край невидимого окна
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // задаём правый нижний край невидимого окна
    ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE_X, WINDOW_SIZE_Y));
    // создаём невидимое окно
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    // получаем список примитивов, которые будут нарисованы
    auto pDrawList = ImGui::GetWindowDrawList();

    // перебираем окружности из динамического массива окружностей
    long long ind=-1;
    for (auto circle: circles) {
        ind++;
        ImColor clr;
        if (ind==ind_c and nado_li)
            clr = ImColor(1, 150, 1);
        else
            clr = ImColor(200, 100, 150);

        // добавляем в список рисования круг
        pDrawList->AddCircle(
                sf::Vector2i(circle.pos.x, circle.pos.y),
                pow(pow(circle.pos2.x-circle.pos.x,2)+pow(circle.pos2.y-circle.pos.y,2),0.5),
                clr,
                100
        );
    }
    // перебираем углы из динамического массива углов
    ind=-1;
    for (auto angle: angles) {
        ind++;
        ImColor clr;
        if (ind==ind_a and nado_li)
            clr = ImColor(1, 150, 1);
        else
            clr = ImColor(10, 100, 150);

        // рисуем угол
        // вершина угла
        sf::Vector2i pointA = angle.pos1;
        // опорные точки
        sf::Vector2i pointB = angle.pos2;
        sf::Vector2i pointC = angle.pos3;

        // определяем вектор смещения
        sf::Vector2i AB = subtract(pointB, pointA);
        sf::Vector2i AC = subtract(pointC, pointA);


        // получаем максимальную длину отрезка на экране, как длину диагонали экрана
        float maxDistance = std::sqrt(WINDOW_SIZE_X * WINDOW_SIZE_X + WINDOW_SIZE_Y * WINDOW_SIZE_Y);

        // получаем новые точки для рисования, которые гарантируют, что линия
        // будет нарисована до границ экрана
        sf::Vector2i renderPointB = sum(pointA, toInt(mul(toFloat(AB), maxDistance)));
        sf::Vector2i renderPointC = sum(pointA, toInt(mul(toFloat(AC), maxDistance)));


        // рисуем линию
        pDrawList->AddLine(
                pointA,
                renderPointB,
                clr,
                0.5f
        );


        // рисуем линию
        pDrawList->AddLine(
                pointA,
                renderPointC,
                clr,
                0.5f
        );

        // рисуем точку
        pDrawList->AddCircleFilled(
                pointA,
                3,
                clr
        );
        // рисуем точку
        pDrawList->AddCircleFilled(
                pointB,
                3,
                clr
        );
        // рисуем точку
        pDrawList->AddCircleFilled(
                pointC,
                3,
                clr
        );
    }

    if (nado_li){
        for (long int i=0;i<P_circuit.size();i++){
            ImColor clr;
            clr = ImColor(255, 255, 1);
            // рисуем линию
            pDrawList->AddLine(
                    P_circuit[i].pos,
                    P_circuit[(i+1)%P_circuit.size()].pos,
                    clr,
                    1.0f
            );
        }
    }
    // заканчиваем рисование окна
    ImGui::End();
}

//случайные объекты
void ShowRandomObjects(){
    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Random Objects"))
        // заканчиваем выполнение
        return;
    // фиксируем id равный 0 для первого элемента
    ImGui::PushID(0);
    // если нажата кнопка `Circle`
    if (ImGui::Button("Circle")) {
        // добавляем в список 1 окружность
        circles.emplace_back(Circle::randomCircle());
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // говорим imGui, что следующий элемент нужно рисовать на той же линии
    ImGui::SameLine();
    // задаём id, равный одному
    ImGui::PushID(1);
    // если нажата кнопка `Angle`
    if (ImGui::Button("Angle")) {
        // добавляем в список угол
        angles.emplace_back(Angle::randomAngle());
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}
// ручное добавление элементов
void ShowAddElem() {
    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Add Elem"))
        // заканчиваем выполнение
        return;

    // фиксируем id равный 0 для первого элемента
    ImGui::PushID(0);
    // если нажата кнопка `Circle`
    if (ImGui::Button("Circle")) {
        // добавляем в список окружность
        circles.emplace_back(Circle(sf::Vector2i(fAddPosBufL[0], fAddPosBufL[1]), sf::Vector2i(sAddPosBufL[0], sAddPosBufL[1])));
        fAddPosBufL[0]=-1;
        sAddPosBufL[0]=-1;
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // говорим imGui, что следующий элемент нужно рисовать на той же линии
    ImGui::SameLine();
    // задаём id, равный одному
    ImGui::PushID(1);
    // если нажата кнопка `Angle`
    if (ImGui::Button("Angle")) {
        // добавляем в список угол
        angles.emplace_back(Angle(sf::Vector2i(fAddPosBufR[0], fAddPosBufR[1]), sf::Vector2i(sAddPosBufR[0], sAddPosBufR[1]),sf::Vector2i(tAddPosBufR[0], tAddPosBufR[1])));
        tAddPosBufR[0]=-1;
        fAddPosBufR[0]=-1;
        sAddPosBufR[0]=-1;
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}



// работа с файлами
void ShowFiles() {
    // если не раскрыта панель `Files`
    if (!ImGui::CollapsingHeader("Files"))
        // заканчиваем выполнение
        return;

    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку загрузки
    if (ImGui::Button("Load")) {
        // загружаем данные из файла
        loadFromFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку сохранения
    if (ImGui::Button("Save")) {
        // сохраняем задачу в файл
        saveToFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

}

// решение задачи
void ShowSolve() {
    // если не раскрыта панель `Solve`
    if (!ImGui::CollapsingHeader("Solve"))
        return;
    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку решения
    if (ImGui::Button("Solve")) {
        solve();
    }

    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);

    // создаём кнопку очистки
    if (ImGui::Button("Clear")) {
        // удаляем всё
        circles.clear();
        angles.clear();
        nado_li=0;
        ind_c=-1;
        ind_a=-1;
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}

// помощь
void ShowHelp() {
    if (!ImGui::CollapsingHeader("Help"))
        return;

    // первый заголовок
    ImGui::Text("ABOUT THIS DEMO:");
    // первый элемент списка
    ImGui::BulletText("Author Yugov Michail 10-1");
    // второй элемент списка
    ImGui::BulletText("Powered by SFML+ImGui");
    // разделитель
    ImGui::Separator();

    // второй заголовок
    ImGui::Text("TASK:");
    // первый элемент списка(многострочный)
    ImGui::BulletText("A set of circles and a set of angles are given on the plane\n"
                      "This program is made to find an angle and a circle\n"
                      "for which the area within their intersection achieves the maximum value\n"
                      "");
    // разделитель
    ImGui::Separator();

}


// главный метод
int main() {
    // создаём окно для рисования
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Geometry Project 10");
    // задаём частоту перерисовки окна
    window.setFramerateLimit(60);
    // инициализация imgui+sfml
    ImGui::SFML::Init(window);

    // задаём цвет фона
    setColor(color);

    // переменная таймера
    sf::Clock deltaClock;
    // пока окно открыто, запускаем бесконечный цикл
    while (window.isOpen()) {
        // создаём событие sfml
        sf::Event event;
        // пока окно принимает события
        while (window.pollEvent(event)) {
            // отправляем события на обработку sfml
            ImGui::SFML::ProcessEvent(event);

            // если событие - это закрытие окна
            if (event.type == sf::Event::Closed) {
                // закрываем окно
                window.close();
            }
            // если событие - это клик мышью
            if (event.type == sf::Event::MouseButtonPressed) {
                // если мышь не обрабатывается элементами imGui
                if (!ImGui::GetIO().WantCaptureMouse) {
                    // если левая кнопка мыши
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        if (fAddPosBufL[0]==-1) {
                            fAddPosBufL[0] = event.mouseButton.x;
                            fAddPosBufL[1] = event.mouseButton.y;
                        } else if (sAddPosBufL[0]==-1) {
                            sAddPosBufL[0] = event.mouseButton.x;
                            sAddPosBufL[1] = event.mouseButton.y;
                        }
                        if (sAddPosBufL[0]!=-1) {
                            circles.emplace_back(sf::Vector2i(fAddPosBufL[0], fAddPosBufL[1]),
                                                 sf::Vector2i(sAddPosBufL[0], sAddPosBufL[1]));
                            fAddPosBufL[0] = -1;
                            sAddPosBufL[0] = -1;
                        }
                    }else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                        if (fAddPosBufR[0]==-1) {
                            fAddPosBufR[0] = event.mouseButton.x;
                            fAddPosBufR[1] = event.mouseButton.y;
                        } else if ( sAddPosBufR[0]==-1){
                            sAddPosBufR[0] = event.mouseButton.x;
                            sAddPosBufR[1] = event.mouseButton.y;
                        } else if (tAddPosBufR[0]==-1){
                            tAddPosBufR[0] = event.mouseButton.x;
                            tAddPosBufR[1] = event.mouseButton.y;
                        }
                        if (tAddPosBufR[0]!=-1) {
                            angles.emplace_back(sf::Vector2i(fAddPosBufR[0], fAddPosBufR[1]),
                                                sf::Vector2i(sAddPosBufR[0], sAddPosBufR[1]),
                                                sf::Vector2i(tAddPosBufR[0], tAddPosBufR[1]));
                            fAddPosBufR[0] = -1;
                            sAddPosBufR[0] = -1;
                            tAddPosBufR[0] = -1;
                        }
                    }
                }
            }
        }

        // запускаем обновление окна по таймеру с заданной частотой
        ImGui::SFML::Update(window, deltaClock.restart());

        // рисование задания
        RenderTask();

        // делаем окно полупрозрачным
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.13f, 0.8f));

        // создаём окно управления
        ImGui::Begin("Control");

        // рисование параметров цвета
        ShowBackgroundSetting();
        // ручное добавление элементов
        ShowAddElem();
        // добавление случайных точек
        ShowRandomObjects();
        // работа с файлами
        ShowFiles();
        // решение задачи
        ShowSolve();
        // помощь
        ShowHelp();

        // конец рисования окна
        ImGui::End();

        // Возвращаем цвет окна к исходному
        ImGui::PopStyleColor();

        // очищаем окно
        window.clear(bgColor);
        // рисуем по окну средствами imgui+sfml
        ImGui::SFML::Render(window);
        // отображаем изменения на окне
        window.display();
    }

    // завершаем работу imgui+sfml
    ImGui::SFML::Shutdown();

    return 0;
}