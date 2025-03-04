# **一、****nullptr**



nullptr 是一个 C++关键字，它是空指针常量，它是用来替代高风险的 NULL 宏和 0 字面量的。所有 跟指针有关的地方都可以用 nullptr，包括函数指针和成员指针。**因此** **nullptr** **专用于空指针，以区 别于** 0。

**传统** **NULL** **的缺点：**有些编译器会把 NULL 定义为空字符，即（（void *）0），也就是 0；有些则会 直接把 NULL 定义为整数 0。例如：对于定义的两个函数 **void f(int)**和 **void f(char \*)**，如果使用 f （0），则编译器不知道去调用哪一个。

![image-20230511005653417](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511005653417.png)

# **二、类型推导：****auto****、****decltype**



C++11 引入了 **auto** 和 **decltype** 这两个关键字实现了类型的推导。即让编译器通过初始值来推算 变量的类型。

​	**auto** 通过变量的初始值来推断变量的类型，auto 不能用于函数传参和推导数组类型。

  **decltype** 是从变量或者表达式中获得当前变量或者表达式的类型，他是解决了 auto 关键字只能 对变量进行类型推导的缺陷。

![image-20230511005736986](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511005736986.png)

**拖尾返回类型（****trailing return type****）**

也就是使用 auto 和 decltype 来推导函数的返回类型，利用 auto 关键字将返回类型后置，这样就可以推导出函数的返回类型

![image-20230511005835742](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511005835742.png)

**结合** **auto** **关键字，实现范围** **for** **的写法**

写一个 for 循环，不比写成那种三段式，直接使用 auto

![image-20230511005900432](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511005900432.png)





# **三、初始化列表（****initializer_list****）**

首先是使用大括号对数组、向量的一些初始化，其次还允许构造函数或其他函数像参数一样使用 初始化列表，这就为类对象的初始化与普通数组等初始化方法提供了统一的桥梁。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511005941082.png" alt="image-20230511005941082" style="zoom:150%;" />



<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511010018686.png" alt="image-20230511010018686" style="zoom:150%;" />



# **四、模板嵌套**



例如我们可以定义有个二维向量 vector，即在一层 vector 中再嵌套一层 vector，这在 C++11 之前 是不能被编译通过的，因为出现连续的右尖括号（C++11 之前**>>**一律当做右移运算符来进行处理的）， 但是在 C++11 中>>变得合法，并且能够顺利通过编译。

![image-20230511010151566](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511010151566.png)















# **五、****Lambda** **表达式**

## **1****、定义说明**

Lambda 表达式是一个匿名函数，也就是没有函数名的函数，我们是直接用它（是一次性的）， 相当于是嵌入式的用它，没有其他地方再用它，这种操作也叫做**闭包（**闭就是封闭的意思，就是其他 地方都不用他，包就是函数**）**。

## **2****、完整的声明格式**

<img src="file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps1.png" alt="img" style="zoom:150%;" />



<img src="file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps2.png" alt="img" style="zoom:150%;" />

**capture list****：**捕获外部变量列表 **params list：**形参列表**

**mutable** **修饰符：**用来说明是否可以修改捕获的外部变量

 **exception****：**异常设定，表示 lambda 表达式是否抛出异常以及何种异常

**return type****：**函数的返回类型

**function body****：**函数体



## **3****、格式中各个部分说明**



###  **[capture list]**：捕捉外部变量列表**

它是 lambda 的引出符（开始标志），编译器可以根据该“标志”来判断出该函数是否为 lambda 函数，同**时捕获列表能够捕捉上下文中的变量作为** **lambda** **函数使用。**

Ø  **[ ]****：**不捕获任何变量

Ø  **[&]****：**捕获外部作用域中的所有变量，并作为引用，在函数体中使用（按  引用捕获） Ø  **[=]****：**捕获外部作用域中的所有变量，并作为副本，在函数体中使用（按值捕获）。 Ø  **[=,&a]****：**按值捕获外部作用域中所有变量，并按引用捕获 a 变量

Ø  **[val]****：**按值捕获 bar 变量，同时不补获其他变量

Ø  **[this]****：**捕获当前类中的 this 指针，让 lambda 表达式拥有和当前类成员函数同样的访问权

限，如果已经使用了&或者=，则就是默认添加此选项。**捕获** **this** **的目的就是可以在** **lambda**中使用当前类的成员函数和成员变量。**

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511010639446.png" alt="image-20230511010639446" style="zoom:150%;" />



### **（params list）：形参列表**

和 C/C++中的普通函数参数意义一样。该部分是可选的，意味着如果我们不需要进行**参数传递**， 就可以连同“（）”一起省略掉

mutable 关键字为一个修饰符，在默认情况下，lambda 函数总是返回一个 const，而当我们在 形参列表后面加了 mutable 关键字后，则可以取消这种默认常量设置，**当以传值方式捕获外部 变量时，使用** **mutable** **关键字，说明后面表达式体（function body)可以修改捕获变量的值。** **说明：**lambda 中使用了 mutable 修饰符，则“形参列表”是不能省略的，即使参数为空。

![image-20230511010859509](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511010859509.png)



### **return type：函数的返回类型**

和 C/C++中的普通函数返回值类型的性质一样。主要目的是用来追踪 lambda 函数（有返回值 情况下）的返回类型。若 lambda 函数不需要返回值，则可以直接将这部分省略掉

### **{ function body }：函数体**

在函数体中，除了可以使用形参列表中的变量外，还可以使用所有捕获到的外部变量。

**注意：**lambda 函数中，形参列表和函数的返回类型都是可选的部分，而且捕捉外部变量列表和函数 体也可以为空。因此，在某种情况下，C++11 中的简略版本的 lambda 函数可以是这样的：

![image-20230511011240027](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511011240027.png)

## **4** **lamdba**  使用场景和好处**

 在项目开发中，有些函数实现的功能过于简单，并且有可能在整个项目中只使用一次，这个时候

就可以考虑使用 lambda 表达式，这样可以使得代码更加紧凑，更加容易维护；   在使用 STL 中的算法的使用 lambda 表达式

例如：循环打印容器中的数据：**for_each** **用法**

![image-20230511011447476](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511011447476.png)

闭包限制了其他函数的访问，更安全更私有



# **六、****for_each** **的使用**

## **1****、函数的原型**



for_each 其实就是一个**函数模板**

![image-20230511011559213](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511011559213.png)

for_each 用于逐个遍历容器元素，他对迭代器区间[first,last）所指的每一个元素，执行单参数函数

对象 f 所定义的操作。

## **2****、实现对比**

**对比：**最原始的做法是使用 for 和迭代器进行遍历的。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511011636337.png" alt="image-20230511011636337" style="zoom:150%;" />

# **七、智能指针**

## **1****、****C++11** **新增的智能指针**

shared_ptr、weak_ptr 和 unique_ptr（替换 auto_ptr，这个 auto_ptr 已经被 C++11 弃用）

## **2****、智能指针出现的原因（****内存泄漏出现的情况****）**

​    1）使用普通指针的时候，容易造成**堆内存**（malloc/new 申请的内存在堆上）的泄漏，即手动 malloc/new 出来的资源，容易忘记使用 free/delete 进行内存的释放；

（2）异常安全问题，例如在 new/malloc 和 delete/free 之间存在抛异常，无法释放内存。这还是会存在内存泄漏的。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511011918974.png" alt="image-20230511011918974" style="zoom:150%;" />

（3）影响执行流的地方要注意释放内存，例如将 free/delete 放在 return 之后，这样就会导致内存泄漏。



## **3****、什么是智能指针以及智能指针的好处**

智能指针本质是类模板，他是利用一种叫做 RAII（资源分配即初始化）的技术对普通的指针进 行封装，这使得智能指针实质是一个对象，行为表现的却像一个指针。

**RAII** **技术：**RAII 技术是一种利用对象生命周期来控制程序资源（例如内存等）的技术。我们要 使用某一个智能指针，就必须去对类进行实例化，得到一个对象（在 C++语言中,对象的类型被称为 类,类代表了某一批对象的共性和特征。 类是对象的抽象,而对象是类的具体实例。）在对象构造时获 取资源，接着控制对资源的访问使之在对象的生命周期内始终保持有效，当类对象的声明周期结束时， 自动调用析构函数释放资源。实际上是把管理一份资源的责任托管给了一个对象，**这样做的好处是：**

（1）不需要显示的释放资源（即使用 free、delete）、

（2） 采用这种方式，对象所需的资源在其生命期内始终保持有效。

## **4****、shared_ptr**

### **（****1****）****shared_ptr** **定义**

**采用引用计数的智能指针**

shared_ptr 以引用计数的方式来管理指针指向资源的生命周期。shared_ptr 表现出的是多个 指针指向相同的对象。shared_ptr 使用“**引用计数****“****模型**，每一个 shared_ptr 的拷贝都指向相同的 内存。每使用他一次，内部的引用计数加 1，每析构一次，内部的引用计数减 1，当计数减为 0 时， 自动删除所指向的堆内存（当最后一个指向动态对象的 shared_ptr 销毁时，会自动销毁其所指对象（通过 delete 操作符））。

![image-20230511012143363](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012143363.png)

### **2****）如何获得****/****创建** **shared_ptr**



Ø  **通过一个指向堆上申请的空间的指针进行初始化**

![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps4.png)



Ø  **通过** **make_shared** **函数得到**

![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps5.png)



Ø  **通过另外一个智能指针对其进行初始化**

![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps6.png)



### **（****3****）****shared_ptr** **的一些注意事项**



Ø  **禁止纯指针给智能指针赋值或者拷贝构造**shared_ptr** **多次引用同一数据，会导致两次释放同一内存****



![image-20230511012304591](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012304591.png)



![image-20230511012408435](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012408435.png)



### **（****4****）****shared_ptr** **循环引用导致内存泄漏**



**循环引用的场景：**如二叉树中父节点与子节点的循环引用，容器与元素之间的循环引用等。 Ø  “**循环引用**”简单来说就是：两个对象互相使用一个 shared_ptr 成员变量指向对方的会造成循环引用。**导致引用计数失效。**



<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012430708.png" alt="image-20230511012430708" style="zoom:150%;" />

上面的代码运行结果为：sa use count:2， 注意此时 sa,sb 都没有释放，产生了内存泄露问题！！！ **即** **A** **内部有指向** **B****，****B** **内部有指向** **A****，这样对于** **A****，****B** **必定是在** **A** **析构后** **B** **才析构，对于** **B****，****A** **必 定是在** **B** **析构后才析构** **A****，这就是循环引用问题，违反常规，导致内存泄露。**

Ø  **解决循环引用的方法**



Ø  当只剩下最后一个引用的时候需要手动打破循环引用释放对象。

Ø  当 A 的生存期超过 B 的生存期的时候，B 改为使用一个普通指针指向 A。

Ø  使用弱引用的智能指针（weak_ptr）打破这种循环引用。

虽然这三种方法都可行，但方法 1 和方法 2 都需要程序员手动控制，麻烦且容易出错。我们一般使 用第三种方法：弱引用的智能指针 weak_ptr。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012515103.png" alt="image-20230511012515103" style="zoom:150%;" />

**强引用和弱引用**

一个强引用当被引用的对象活着的话，这个引用也存在（就是说，当至少有一个强引用，那么这个对 象就不能被释放）。share_ptr 就是强引用。相对而言，弱引用当引用的对象活着的时候不一定存在。 仅仅是当它存在的时候的一个引用。弱引用并不修改该对象的引用计数，这意味这弱引用它并不对对 象的内存进行管理，在功能上类似于普通指针，然而一个比较大的区别是，弱引用能检测到所管理的 对象是否已经被释放，从而避免访问非法内存。



## **5****、****weak_ptr**

**（****1****）****weak_ptr** **的定义和说明**



weak_ptr 是为了配合 shared_ptr 而引入的一种智能指针，它的最大作用在于协助 shared_ptr 工作，像旁观者那样观测资源的使用情况。weak_ptr 可以从一个 shared_ptr 或者另一个 weak_ptr 对象构造，获得资源的观测权。**weak_ptr** **没有共享资源，它的构造不会引起指针引用 计数的增加。**使用 weak_ptr 的成员函数 use_count()可以观测资源的引用计数，另一个成员函 数 **expired()**的功能等价于 use_count()==0,但更快，表示被观测的资源(也就是 shared_ptr 的 管理的资源)已经不复存在。weak_ptr 可以使用一个非常重要的成员函数 lock()从被观测的 shared_ptr 获得一个可用的 shared_ptr 对象， 从而操作资源。但当 expired()==true 的时候， 即 weak_ptr 所指向的资源已经不存在了，lock()函数返回一个“空”shared_ptr**（**一个存储空 指针的 shared_ptr）。**因此我们可以使用** **lock** **函数判断** **weak_ptr** **所指向的资源是否被回收。**

![image-20230511012719996](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012719996.png)

## **6****、****unique_ptr**



### **（****1****）定义和说明**

**资源所有权独占的智能指针，**只允许基础指针的一个所有者。 可以通过**移动语义**移到新所有者， 没有实现拷贝和赋值语义，即我们无法得到指向同一个对象的两个 unique_ptr，

![image-20230511012746679](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012746679.png)

### **（****2****）通过** **move** **语义转移资源所有权**

移动语义可以通过 **move** **语义**实现的，使用“**move** **语义**”可以将当前资源（申请的堆内存）所 有权从一个对象转移到另一个对象。

![image-20230511012810698](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511012810698.png)

### **（****3****）** **unique_ptr** **的其他操作**

 **ptr1.reset();**

ptr1 会释放当前他所管理的对象 

  **ptr1.reset(ptr2);**

ptr1 会释放当前他所管理的对象，获取 ptr2 的资源管理权限

 **ptr.release();**

通过 release 函数释放资源（堆内存）的所有权，返回一个指向 ptr 所管理的对象的原生针。

## **7****、总结：智能指针的原理**





**独占式** **unique_ptr** **和共享式** **shared_ptr**

![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps8.png)

Ø  **shared_ptr** **总结说法** **1**

shared_ptr 可以在多个对象之间共享一个原始指针的内存区域，每个复制都会使其中的引用计数 1，而智能指针对象析构时，将会使引用计数-1，当引用计数从 1 变为 0 时，表示没有任何对象 在使用该指针了，因此会调用 delete 释放相应的空间。引用计数可以是一个整数或者结构体， 多个智能指针内部共享一个引用计数的指针变量，通过指针操作该变量的值，共同维护一个正确 的引用数值。

Ø  **shared_ptr** **总结说法** **2**

shared_ptr 采用了引用计数器，多个 shared_ptr 中的原始指针指向同一个内存区域（同一个对 象），并共同维护同一个引用计数器。引用计数是实现智能指针的一种通用方法。智能指针将一个计数器与类指向的对象相关联，引用计数跟踪共有多少个类对象共享同一指针。具体做法如下：   当创建类的新对象时，初始化指针，并将引用计数设置为 1，

当对象作为另一个对象的副本时，复制构造函数复制副本指针，并增加与指针相应的引用计数（加 1）  使用赋值操作符对一个对象进行赋值时，处理复杂一点：先使左操作数的指针的引用计数减 1（为何减 1：因为指针已经指向别的地方），如果减 1 后引用计数为 0，则释放指针所指对 象内存。然后增加右操作数所指对象的引用计数（为何增加：因为此时做操作数指向对象即 右操作数指向对象）。

析构函数：调用析构函数时，析构函数先使引用计数减 1，如果减至 0 则 delete 对象。

### 	Ø  **unique_ptr** **总结**

unique_ptr 是唯一持有某一个原始指针，某个时刻只能有一个 unique_ptr 指向一个给定的对象， 它不能与其他 unique_ptr 类型的指针对象共享所指对象的内存。unique_ptr 是一个删除了拷贝 构造函数、保留了移动构造函数的指针封装类型，故这种”所有权”仅能够通过标准库的 move 函数来转移，其中并没有对原始对象进行拷贝，而只是将原始指针的所有权移动到另一个智能指 针。

可以通过调用 release 或 reset 将指针的所有权从一个(非 const)unique_ptr 转移给另一个 unique。调用 release 会切断 unique_ptr 和它原来管理的对象间的联系。release 返回的指针 通过被用来初始化另一个智能指针或给另一个智能指针赋值。如果不用另一个智能指针来保存 release 返回的指针，程序就要负责资源的释放。

### Ø  **weak_ptr** **总结**

weak_ptr 是为配合 shared_ptr 而引入的一种智能指针来协助 shared_ptr 工作，它可以从一个 shared_ptr 或另一个 weak_ptr 对象构造，它的构造和析构不会引起引用计数的增加或减少。他 可以使用 lock 获得一个可用的 shared_ptr 对象。

## **8****、智能指针用于标准容器**

（1）**将容器作为智能指针的（例如** **shared_ptr****）管理对象。**例如：**shared_ptr<list<T> >**,使容 器可以安全的共享。

（2）**将智能指针****(****例如** **shared_ptr)****作为容器的元素。**如 **vector<shared_ptr<T> >** ,因为， shared_ptr 支持拷贝语言和比较操作，符合标准容器对元素的操作，所以可以在容器中安全的容纳 元素指针，而不是拷贝。

给出一段将 shared_ptr 用于标准库的代码

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511013641566.png" alt="image-20230511013641566" style="zoom:150%;" />

## **10****、智能指针和线程安全的问题**

**<1>**  **智能指针** **shared_ptr** **本身（底层实现原理是引用计数）是线程安全的**

智能指针的引用计数在手段上使用了 **atomic** **原子操作**，只要 shared_ptr 在拷贝或赋值时增加 引用，析构时减少引用就可以了。首先原子是线程安全的，所有智能指针在多线程下引用计数也 是安全的，也就是说智能指针在多线程下传递使用时引用计数是不会有线程安全问题的。

**<2>**  **智能指针指向的对象的线程安全问题，智能指针没有做任何保障**

Ø  **遇到的问题**

对于智能指针shared_ptr的引用计数本身是安全且无锁的，但**对象的写则不是**，因为 shared_ptr 有两个数据成员，一个是指向的对象的指针，还有一个就是我们上面看到的引用计数管理对象， 当智能指针发生拷贝的时候，标准库的实现是先拷贝智能指针，再拷贝引用计数对象，拷贝引用 计数对象的时候，会使 use_count 加一，这个操作并不是原子操作，隐患就出现在这里。假设引 用计数原来是 1，**两个线程中智能指针的引用计数** **use_count** **同时****++**或**--**，可能还是** **2****（预期 应该是** **3****），这样引用计数就错乱了，违背了原子性。**

**分析：**如两个线程操作同一变量过程中，一个线程执行过程中可能被内核临时挂起，这就是线程 切换，当内核再次切换到该线程时，之前的数据可能已被修改，不能保证原子操作。

### Ø  **下多线程编程中的三个核心概念，可以作为面试中原因分析的讲解**

#### ü  **原子性的举例**

这一点，跟数据库事务的原子性概念差不多，即一个操作（有可能包含有多个子操作） 要么全部执行（生效），要么全部都不执行（都不生效）。关于原子性，一个非常经典的例子就是银行转账问题：比如 A 和 B 同时向 C 转账 10 万元。如果转 账操作不具有原子性，A 在向 C 转账时，读取了 C 的余额为 20 万，然后加上转账的 10 万，计算出此时应该有 30 万，但还未来及将 30 万写回 C 的账户，此时 B 的转账请求过来了，B 发现 C 的余额 为 20 万，然后将其加 10 万并写回。然后 A 的转账操作继续——将 30 万写回 C 的余额。这种情况下 C 的最终余额为 30 万，而非预期的 40 万。



#### ü  **可见性的举例**

可见性是指，当多个线程并发访问共享变量时，一个线程对共享变量的修改，其它 线程能够立即看到。可见性问题是好多人忽略或者理解错误的一点。

CPU 从主内存中读数据的效率相对来说不高，现在主流的计算机中，都有几级缓存。 每个线程读取共享变量时，都会将该变量加载进其对应 CPU 的高速缓存里，修改该变量后， CPU 会立即更新该缓存，但并不一定会立即将其写回主内存（实际上写回主内存的时间不可 预期）。此时其它线程（尤其是不在同一个 CPU 上执行的线程）访问该变量时，从主内存中 读到的就是旧的数据，而非第一个线程更新后的数据。

这一点是操作系统或者说是硬件层面的机制，所以很多应用开发人员经常会忽略。

#### ü  **顺序性举例**

顺序性指的是，程序执行的顺序按照代码的先后顺序执行。处理器为了提高程序整 体的执行效率，可能会对代码进行优化，其中的一项优化方式就是调整代码顺序，按照更高 效的顺序执行代码。讲到这里，有人要着急了——什么，CPU 不按照我的代码顺序执行代码， 那怎么保证得到我们想要的效果呢？实际上，大家大可放心，CPU 虽然并不保证完全按照代 码顺序执行，但它会保证程序最终的执行结果和代码顺序执行时的结果一致。

#### Ø  **解决办法****——****加入互斥锁**

使用**互斥锁**对多线程读写同一个 shared_ptr 进行加锁操作（多个线程访问同一资源时，为了保 证数据的一致性，最简单的方式就是使用 mutex（互斥锁））

一旦一个线程获得了锁对象，那么在临界区时一直是受保护的，具体表现为该线程一直占着资源不放。

#### ü  **临界区的说明**

有时我们会遇到两个进/线程共同使用同一个资源的情况，这个资源就称为临界区。 临界区是指某一时间只能有一个线程执行的一个代码段

#### Ø  **加入互斥锁的代码展示**

ü  **方法** **1****：**直接操作 mutex，即直接调用 mutex 的 lock / unlock 函数

![image-20230511014049517](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511014049517.png)

**方法** **2****：**使用 lock_guard 自动加锁、解锁。原理是 RAII，和智能指针类似

**C++****利用了一个非常好的特性：**当一个对象初始化时自动调用构造函数，当一 个对象到达其作用域结尾时，自动调用析构函数。所以我们可以利用这个特性解决锁 的维护问题：把锁封装在对象内部！此时，在构造函数时获得锁，在语句返回前自动 调用析构函数释放锁。其实这种做法有个专有的名称，叫做 RAII

![image-20230511014153166](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511014153166.png)

## **11****、原子操作****<atomic>**

### **（****1****）概念**

Ø  原子操作，它表示在多个线程访问同一个全局资源的时候，能够确保所有其他线程都不在同一时 间访问该资源。也就是确保了在同一时刻只有一个线程对这个资源进行访问。

Ø  项目中经常用遇到多线程操作共享数据问题，常用的处理方式是对共享数据进行加锁，如果多线 程操作共享变量也同样采用这种方式。

Ø  **为什么要对共享变量加锁或使用原子操作？如两个线程操作同一变量过程中，一个线程执行过程 中可能被内核临时挂起，这就是线程切换，当内核再次切换到该线程时，之前的数据可能已被修 改，不能保证原子操作。**

Ø  **新标准** **C++11**，引入了原子操作的概念，并通过这个新的头文件提供了多种原子操作数据类型， 例如，atomic_bool,atomic_int 等等，相比加锁机制 mutex.lock()，mutex.unlock()，性能有 几倍的提升。

如果我们在多个线程中对这些类型的共享资源进行操作，编译器将保证这些操作都是原子性的， 也就是说，确保任意时刻只有一个线程对这个资源进行访问，编译器将保证，多个线程访问这个 共享资源的正确性。从而避免了锁的使用，提高了效率。

### **（****2****）加锁机制实现原子操作、****C++11** **实现原子操作对比**

**atomic** **原子操作实现**

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511014427957.png" alt="image-20230511014427957" style="zoom:150%;" />

**总结：**c++11 的原子类 atomic 相比使用加锁机制性能有 2~3 倍提升，对于共享变量能用原子类型的就不要再用加锁机制了。

## **12****、处理器如何实现原子操作**

**1)** **使用总线锁保证原子性**

如果多个处理器同时对共享变量进行读写操作,那么共享变量就会被多个处理器同时进行操作,这样读 写操作就不是原子的,操作完之后共享变量的值会和期望的不一致.

所谓总线锁就是使用处理器提供的一个 LOCK#信号,当一个处理器在总线上输出次信号时,其他处理 器的请求将被阻塞住,那么该处理器可以独占共享内存.

在 x86 平台上，CPU 提供了在指令执行期间对总线加锁的手段。CPU 芯片上有一条引线#HLOCK pin， 如果汇编语言的程序中在一条指令前面加上前缀"LOCK"，经过汇编以后的机器代码就使 CPU 在执行 这条指令的时候把#HLOCK pin 的电位拉低，持续到这条指令结束时放开，从而把总线锁住，这样同 一总线上别的 CPU 就暂时不能通过总线访问内存了，保证了这条指令在多处理器环境中的。

**(2)** **使用缓存锁保证原子性**

通过缓存锁定保证原子性。在同一时刻我们只需保证对某个内存地址的操作是原子性即可，但总线锁 定把 CPU 和内存之间通信锁住了，这使得锁定期间，其他处理器不能操作其他内存地址的数据，所 以总线锁定的开销比较大。

所谓“缓存锁定”就是如果缓存在处理器缓存行中内存区域在 LOCK 操作期间被锁定，当它执行锁操 作回写内存时，处理器不在总线上声言 LOCK＃信号，而是修改内部的内存地址，并允许它的缓存一



致性机制来保证操作的原子性，因为缓存一致性机制会阻止同时修改被两个以上处理器缓存的内存区 域数据，当其他处理器回写已被锁定的缓存行的数据时会起缓存行无效。

但是有两种情况下处理器不会使用缓存锁定。第一种情况是：当操作的数据不能被缓存在处理器内部， 或操作的数据跨多个缓存行（cache line），则处理器会调用总线锁定。第二种情况是：有些处理器 不支持缓存锁定。对于 Inter486 和奔腾处理器,就算锁定的内存区域在处理器的缓存行中也会调用总 线锁定。

# **八、右值引用**

## **1****、什么是左值和右值**



Ø  **左值**

可被引用的数据对象，例如：变量、数组元素、结构成员等。常规变量属于可修改的左值，const 变量属于不可修改的左值。

Ø  **右值**

字面常量、包含多项的表达式以及返回值的函数（条件是该函数返回的不是引用）

**右值特性：**允许调用成员函数、只能被 const reference 指向、右值不能当成左值使用，但左值 可以当成右值使用（左值可以出现在赋值符的左边和右边，而右值只能出现在赋值符 的右边）。

Ø  **左值右值的区别**

**右值就是一个临时变量，**只是临时地址空间，左值有其他地址空间，**换句话说，使用取地址符****&**对某个值取地址，如果取不到地址，则是右值。**

## **2****、什么是右值引用和左值引用**

Ø  **右值引用**

为了支持**移动操作**，C++11 引入了一种新的引用类型，即**右值引用，**右值引用就是对右值进行引用操作，通过**&&**来获得右值引用。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511014914560.png" alt="image-20230511014914560" style="zoom:150%;" />

 **左值引用**

为了与右值引用区分，对于常规引用，我们称之为左值引用

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511014945625.png" alt="image-20230511014945625" style="zoom:150%;" />

## **3****、右值引用应用于移动构造函数实现** **C++11** **的移动语义**

对于有些**临时对象**，对其执行拷贝操作，拷贝完了就没有什么用了，这样就会造成没有意义的资源申 请和释放操作，如果能够直接使用临时对象已经申请的资源（内存），即能节省资源，又能节省资源 申请和释放的时间。

C++11 新增的移动语义就能够做到这一点，实现移动语义就要使用移动构造函数和移动赋值运算符。 拷贝构造函数中传入的参数是一个左值引用，而移动构造函数中传入的参数是一个右值引用。**我们所 要处理的是一个临时对象，他是一个右值，**他会优先进入移动构造函数而不是拷贝构造函数，与拷贝 构造函数不同，移动构造函数不会分配任何新内存，他是将自己的指针指向临时对象的资源（内存）， 然后将临时对象的指针置为 nullptr（**置位** **nullptr** **这一步很重要，如果不将临时对象的指针置为空， 那么临时对象析构的时候就会释放掉这块资源，从而导致我们刚才的移动是白费的**）。最终源对象（临时对象）被销毁，也就是在其上运行析构函数。

![image-20230511015111616](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511015111616.png)

​                   																				**拷贝构造和移动构造的对比**

```cpp
#include <iostream> #include <cstring> #include <vector> using namespace std;
class MyString
{
public:
static size_t CCtor; //统计调用拷贝构造函数的次数 static size_t MCtor; //统计调用移动构造函数的次数 static size_t CAsgn; //统计调用拷贝赋值函数的次数 static size_t MAsgn; //统计调用移动赋值函数的次数
public: // 构造函数
MyString(const char* cstr=0){
if (cstr) {
m_data = new char[strlen(cstr)+1]; strcpy(m_data, cstr);
}
else {
m_data = new char[1];
*m_data = '\0';
}
}
// 拷贝构造函数
MyString(const MyString& str) {
CCtor ++;
m_data = new char[ strlen(str.m_data) + 1 ]; strcpy(m_data, str.m_data);
}
// 移动构造函数
MyString(MyString&& str) noexcept
:m_data(str.m_data) {
MCtor ++;
str.m_data = nullptr; //不再指向之前的资源了 }
// 拷贝赋值函数 =号重载
MyString& operator=(const MyString& str){ CAsgn ++;
if (this == &str) // 避免自我赋值!!
return *this;
delete[] m_data;
m_data = new char[ strlen(str.m_data) + 1 ]; strcpy(m_data, str.m_data);
return *this;
}
// 移动赋值函数 =号重载
MyString& operator=(MyString&& str) noexcept{ MAsgn ++;
if (this == &str) // 避免自我赋值!! return *this;
delete[] m_data;
m_data = str.m_data;
str.m_data = nullptr; //不再指向之前的资源了 return *this;
}
~MyString() {
delete[] m_data; }
char* get_c_str() const { return m_data; } private:
char* m_data;
};
size_t MyString::CCtor = 0;
size_t MyString::MCtor = 0;
size_t MyString::CAsgn = 0;
size_t MyString::MAsgn = 0;
int main()
{
vector<MyString> vecStr;
vecStr.reserve(1000); //先分配好 1000 个空间
for(int i=0;i<1000;i++){
vecStr.push_back(MyString("hello"));  //调用的是移动构造函数 }
cout << "CCtor = " << MyString::CCtor << endl;
cout << "MCtor = " << MyString::MCtor << endl;
cout << "CAsgn = " << MyString::CAsgn << endl;
cout << "MAsgn = " << MyString::MAsgn << endl;
}
结果 CCtor =  0 MCtor =  1000 CAsgn =  0 MAsgn =  0
```

## **4****、标准库** **move** **函数：实现左值转换为对应的右值引用类型，从而实现移动语义**

该函数在#include <utility>头文件中。

对于有些对象为左值的情况，起初他肯定是只能调用拷贝构造函数来实现拷贝，但是有些左值是局部变量，他的生命周期很短，因此我们想要对其应用移动转移资源，而非拷贝，这时候就需要用到 C++11 中的 move 函数，他可以获得绑定到左值上的右值引用，这样就可以将这个右值引用作为移动构造函数的传入参数，从而实现移动语义。

```cpp
   实例 1：
   int main() 
{
vector<MyString> vecStr;
vecStr.reserve(1000); //先分配好 1000 个空间 for(int i=0;i<1000;i++){
MyString tmp("hello");
vecStr.push_back(tmp); //调用的是拷贝构造函数 }
cout << "CCtor = " << MyString::CCtor << endl; cout << "MCtor = " << MyString::MCtor << endl; cout << "CAsgn = " << MyString::CAsgn << endl; cout << "MAsgn = " << MyString::MAsgn << endl;
cout << endl;
MyString::CCtor = 0;
MyString::MCtor = 0;
MyString::CAsgn = 0;
MyString::MAsgn = 0;
vector<MyString> vecStr2;
vecStr2.reserve(1000); //先分配好 1000 个空间
for(int i=0;i<1000;i++){
MyString tmp("hello");
vecStr2.push_back(std::move(tmp)); //调用的是移动构造函数 }
cout << "CCtor = " << MyString::CCtor << endl;
cout << "MCtor = " << MyString::MCtor << endl;
cout << "CAsgn = " << MyString::CAsgn << endl;
cout << "MAsgn = " << MyString::MAsgn << endl;
}
运行结果 CCtor =  1000
MCtor =  0
CAsgn =  0
MAsgn =  0
CCtor =  0
MCtor =  1000
CAsgn =  0
MAsgn =  0
```

  **实例** **2****：**

如果 move( )函数使用得当，它的效果是巨大的，STL 库中的数据交换函数 std::swap( )就是使用这个操作实现的，其示意性代码如下所示：

```cpp
void swap(T &a, T &b) 
{
T tmp = move(a);
a = move(b);
b = move(tmp);
}
```

如上所示，如果 T 是一个类类型，里面有包含了大量资源，那么不用转移语义技术进行这种交换会 耗费非常大的资源。而在这里，仅仅通过交换了 3 次指针便完成了任务，具有极高的效率。

## **5****、右值引用之****——****参数完美转发模板****——forward**

**完美转发的规则**

在程序设计实践中，函数模板经常需要调用一些外部函数实现它的功能，这时就需要利用模板为 这些函数传递参数，对于模板来说，它的任务就是对参数的完美转发，即一不能改变参数特性（左值、 右值），二是不能产生额外的开销（即转发的参数必须是**引用类型（此为引用传递，虽然也会进行拷 贝，但只是地址的拷贝）**，因为**值传递**会产生一个原参数副本，即会创建一个临时对象并对数据进行 拷贝，会产生额外的消耗）。如果模板能够把所有数据类型的参数都能按照上述要求进行转发，这就 实现了参数完美转发。

**引用符折叠规则**

为了使编译器能够正确处理多个引用符连用的这些参数类型，C++ 11 为模板定义了一些新的引用 类型推倒规则。凡是支持 C++ 11 的编译器一旦在模板的参数推倒中发现具有多个引用符的表达式， 编译器就会按照新定义的规则把引用符截掉，以得到合法且合理的简单引用表达式。 **规则：**

1）所有的右值引用叠加到右值引用上仍然使一个右值引用。

2）所有的其他引用类型之间的叠加都将变成左值引用。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511015445571.png" alt="image-20230511015445571" style="zoom:150%;" />

对于以**右值引用（** **T &&** **）**作为参数的函数模板，还有下面的表所列举的规则。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230511015503543.png" alt="image-20230511015503543" style="zoom:150%;" />

C++ 11 采用了原来就有的数据类型强制转换函数模板 static_cast，于是调用目标函数的代码就写成如下形式：



![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps9.png)

在上面的代码中，由于 static_cast 的模板参数与 Tmp 模板参数类型完全相同，都为 T &&，所以 保证了参数的输入类型与函数的实收参数类型完全一致。进来的是右值，发到噶不是故意的一定也是 右值（ 这是 static_cast 转换的结果 ）；进来的是左值，发到函数的也一定是左值。 其实，static_cast 的这个转换也只是对参数为右值时才有用，当参数为左值是，static_cast 转换其 实并没有起到作用。通过前面已经知道，这时候 static_cast 只是把已经是左值的 a 转换为左值。 为了区别 move( ) 和 static_cast，并且使之更具有语义性，C++11 将 static_cast 封装成了函数 模板 std::farward。对于上面的代码，正确的书写方法应该如下所示：

**farward** **解释**

根据右值判断的推倒，调用 forward 传出的值，若原来是一个右值，那么他转出来就是一个右值， 否则为一个左值。这样的处理就完美转发了原有参数的左右值属性，不会造成一些不必要的拷贝。 （因为一开始是传入的参数是右值，根据原始的 C++标准定义，这个参数变成了一个左值，那么他 永远不会调用接下来函数的右值版本，这样可能在一些情况下造成拷贝）。

**该函数在****#include <utility>****头文件中。**



![img](file:///C:\Users\ADMINI~1\AppData\Local\Temp\ksohtml29256\wps10.png)