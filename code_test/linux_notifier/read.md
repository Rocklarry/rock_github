模块0-chain0.c
定义两个函数，一个是注册函数register_test_notifier，一个发送事件函数call_test_notifiers

 模块1-chain1.c
定义notifier_block的test_init_notifier，其回调函数为test_init_event。
然后调用模块０中的事件注册函数register_test_notifier,向模块进行事件订阅。当事件发生时会后调用函数test_init_event.


模块2-chain2.c
调用模块０的事件发送函数call_test_notifiers，事件发送后，订阅时间的模块１会调用其自己的函数test_init_event,输出字符串。




sudo insmod 0-chain0.ko
sudo insmod 1-chain1.ko
sudo insmod 2-chain2.ko


[100899.802743] I'm in test_chain_0
[100904.741578] 这个是 test_chain_1
[100908.412878] I'm in test_chain_2
[100908.412880] 获得chain事件: test_chain_2 在初始化！


sudo rmmod 2-chain2.ko
sudo rmmod 1-chain1.ko
sudo rmmod 0-chain0.ko

 cat /proc/kallsyms |grep "chain"
0000000000000000 r __ksymtab_register_test_notifier	[0_chain0]
0000000000000000 r __kstrtab_register_test_notifier	[0_chain0]
0000000000000000 r __ksymtab_call_test_notifiers	[0_chain0]
0000000000000000 r __kstrtab_call_test_notifiers	[0_chain0]




