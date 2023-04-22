#include <thread>
#include <iostream>
#include <chrono>
#include <vector>

// ЧТОБЫ УДОБНО КОМПИЛИРОВАТЬ КОД, РЕАЛИЗУЙТЕ КАЖДОЕ ЗАДАНИЕ В ВИДЕ ФУНКЦИЙ

// 1. Создайте поток, который выведет текущее время с помощью передачи в поток функции print_time()
// Для получения времени используйте функцию std::chrono::system_clock::now(); (1 балл)

void print_time() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current time (1): " << std::ctime(&t_c);
}

void task1(){
    std::thread thread(print_time);
    thread.join();
}
// 2. Создайте объект-функтор, делающий то же самое, только время считайте с помощью std::time (1 балл)

class Time{
public:
    void operator()(){
        {
            std::time_t result = std::time(nullptr);
            std::cout << "Current time (2): " << std::asctime(std::localtime(&result));
        }
    }
};

void task2(){
    Time time_object;
    std::thread thread(time_object);
    thread.join();
};


// 3. Создайте поток, который вернет текущее время по ссылке, используя передачу задачи с помощью лямба-функции
// После завершения потока снова замерьте текущее время и напечатайте пройденное время (разницу в мс). (1 балл)

void task3(){
    std::chrono::time_point<std::chrono::system_clock> t_begin;
    std::chrono::time_point<std::chrono::system_clock> t_end;

    std::thread thread([&t_begin](){t_begin = std::chrono::system_clock::now();});
    thread.join();

    t_end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_begin);
    std::cout << "Duration (3): " << duration.count() * 0.001 << " milliseconds" << std::endl;
}


//4. Создайте столько потоков, сколько доступно на вашем компьютере
// (std::thread::hardware_concurrency())
// Каждый поток должен напечатать свой id. Особое внимание обратите на вывод, чтобы он был согласованным (1 балл)

void do_nothing(){};
//Немного нужно переделать
void task4(){
    size_t threads_n = std::thread::hardware_concurrency();
    std::vector <std::thread> threads_v;
    for (size_t idx = 0; idx < threads_n; idx++){
        threads_v.push_back(std::thread(do_nothing));
    }

    std::cout << "Threads ID (4): ";
    for (size_t idx = 0; idx < threads_n; idx++){
        std::cout << threads_v[idx].get_id() << " ";
        threads_v[idx].join();
    }
    std::cout << std::endl;
}


// 5. Напечатайте числа ПОСЛЕДОВАТЕЛЬНО от 0 до 10 так, чтобы четный числа выводил один поток, а нечетные - другой.
// Требуемый вывод: 0 1 2 3 4 5 6 7 8 9 10. Используйте механизм блокировки. (2 балла)

//Нужно исправить (сейчас это скорее эксперименты)
void task5(){
    int begin = 0;
    int end = 10;

    bool value = true;

    int n = begin;
    std::thread t1([&n, &end]() {
        if (n % 2 == 0 && n <= end) {
            std::cout << n << " ";
            n += 1;
        };
    }
    );
    std::thread t2([&n, &end](){
        if (n % 2 == 0 && n <= end){
            std::cout << n << std::endl;
            n += 1;
        };
    }
    );
    t1.join();
    t2.join();
}

// 6. Создайте две матрицы А и B размером 300х300. Напишите параллельную версию перемножения матриц с записью результата
// в отдельную матрицу С. (3 балла)
// Cоздайте 6 потоков (не считая родительский).  1 поток должен перемножать строки 0-49 на столбцы 0-49,
// 2 поток - строки 50-99 на столбцы 50-99 и так далее.
// Далее одна из вариаций кода переножения матрицы 3х3:
//int i,j,k;
//for (i=1; i<=3; i++)
//    {
//        for (j=1; j<=3; j++)
//        {
//            sum = 0;
//            for (k=1; k<=3; k++)
//              sum = sum + A[i][k]*B[k][j];
//        }
//    C[i][j] = sum;
//    }
//}

//7. Замерьте время работы параллельного перемножения матриц и последовательного кода.
// Сравните и объясните результаты. (2 балла)

int main(){
    task1();
    task2();
    task3();
    task4();
    task5();
    return 0;
}
