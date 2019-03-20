HW03
===
This is the hw03 sample. Please follow the steps below.

# Build the Sample Program

1. Fork this repo to your own github account.

2. Clone the repo that you just forked.

3. Under the hw03 dir, use:

	* `make` to build.

	* `make clean` to clean the ouput files.

4. Extract `gnu-mcu-eclipse-qemu.zip` into hw03 dir. Under the path of hw03, start emulation with `make qemu`.

	See [Lecture 02 ─ Emulation with QEMU] for more details.

5. The sample is a minimal program for ARM Cortex-M4 devices, which enters `while(1);` after reset. Use gdb to get more details.

	See [ESEmbedded_HW02_Example] for knowing how to do the observation and how to use markdown for taking notes.

# Build Your Own Program

1. Edit main.c.

2. Make and run like the steps above.

3. Please avoid using hardware dependent C Standard library functions like `printf`, `malloc`, etc.

# HW03 Requirements

1. How do C functions pass and return parameters? Please describe the related standard used by the Application Binary Interface (ABI) for the ARM architecture.

2. Modify main.c to observe what you found.

3. You have to state how you designed the observation (code), and how you performed it.

	Just like how you did in HW02.

3. If there are any official data that define the rules, you can also use them as references.

4. Push your repo to your github. (Use .gitignore to exclude the output files like object files or executable files and the qemu bin folder)

[Lecture 02 ─ Emulation with QEMU]: http://www.nc.es.ncku.edu.tw/course/embedded/02/#Emulation-with-QEMU
[ESEmbedded_HW02_Example]: https://github.com/vwxyzjimmy/ESEmbedded_HW02_Example

--------------------

- [ ] **If you volunteer to give the presentation next week, check this.**

--------------------

**★★★ Please take your note here ★★★**


## **實驗目的**
1. 撰寫c function call程式，並觀察argument傳遞及function return parameter在組語、暫存器及記憶體上的變化。


## **實驗步驟**
1. **程式撰寫**: 轉寫c function call程式
2. **make編譯程式**: 以 `**make**` 指令編譯程式
3. **objdump反組譯**: 執行objdump指令反組譯程式，並觀察組合語言的內容。objdump指令
4. **qemu虛擬機計算逐步執行**: 以qemu虛擬機觀察c function程式在暫存器上的執行結果。


----------
## **撰寫程式**

main.c程式包含有 `**add(int a, int b) function**`，及`**a**`, `**b**`, `**c**`, `**d**`四個變數。

**變數設計**
`**a**`變數的目的是要測試在進入到`**reset_handler(void)**`函式之前，是否可以在暫存器或記憶體空間中找到`**a**`變數。`**b**`和`**c**`變數的設計目的在找到被分配到stack 記憶體所在的位置。`**d**`變數用於確認進入到`**while(1)**`迴圈之前的所在位置，以及最終變數會被寫入到記憶體的位置。

**函數設計**
為了觀察記憶體分配、在組合語言中執行順序的跳轉，設計有add()函數如下:

    int add(int a, int b){
    
        return a+b;
    }

由c語言的設計原理，此處的a和b因為不是指標，所以會被分配額外的記憶體空間，在`**a+b**`計算完成後，會把計算結果回傳到呼叫此函式的`**reset_handler(void)**`中，


    int add(int a, int b);
    int a=1; //Glbal varable
    void reset_handler(void)
    {
        int b=2;
        int c=3;
        int dL=add(b,c);
        int d=7; //End of the the rest_handler()
        while (1)
            ;
    }
    
    int add(int a, int b){
    
        return a+b;
    }



----------
## **Objdump反組譯**

在目標資料夾下執行objdump指令

    rm-none-eabi-objdump -D sections.elf

可產生反組譯結果如下:

    Disassembly of section .mytext:
    
    00000000 <reset_handler-0x8>:
       0:        20000100         andcs        r0, r0, r0, lsl #2
       4:        00000009         andeq        r0, r0, r9
    
    00000008 <reset_handler>:
       8:        b580              push        {r7, lr}
       a:        b084              sub        sp, #16
       c:        af00              add        r7, sp, #0
       e:        2302              movs        r3, #2
      10:        60fb              str        r3, [r7, #12]
      12:        2303              movs        r3, #3
      14:        60bb              str        r3, [r7, #8]
      16:        68f8              ldr        r0, [r7, #12]
      18:        68b9              ldr        r1, [r7, #8]
      1a:        f000 f805         bl        28 <add>
      1e:        6078              str        r0, [r7, #4]
      20:        2307              movs        r3, #7
      22:        603b              str        r3, [r7, #0]
      24:        e7fe              b.n        24 <reset_handler+0x1c>
      26:        bf00              nop
    
    00000028 <add>:
      28:        b480              push        {r7}
      2a:        b083              sub        sp, #12
      2c:        af00              add        r7, sp, #0
      2e:        6078              str        r0, [r7, #4]
      30:        6039              str        r1, [r7, #0]
      32:        687a              ldr        r2, [r7, #4]
      34:        683b              ldr        r3, [r7, #0]
      36:        4413              add        r3, r2
      38:        4618              mov        r0, r3
      3a:        370c              adds        r7, #12
      3c:        46bd              mov        sp, r7
      3e:        f85d 7b04         ldr.w        r7, [sp], #4
      42:        4770              bx        lr
    
    Disassembly of section .data:
    
    00000044 <a>:
      44:        00000001         andeq        r0, r0, r1
    
    Disassembly of section .comment:
    
    00000000 <.comment>:
       0:        3a434347         bcc        10d0d24 <a+0x10d0ce0>
       4:        35312820         ldrcc        r2, [r1, #-2080]!        ; 0xfffff7e0
       8:        392e343a         stmdbcc        lr!, {r1, r3, r4, r5, sl, ip, sp}
       c:        732b332e                         ; <UNDEFINED> instruction: 0x732b332e
      10:        33326e76         teqcc        r2, #1888        ; 0x760
      14:        37373131                         ; <UNDEFINED> instruction: 0x37373131
      18:        2029312d         eorcs        r3, r9, sp, lsr #2
      1c:        2e392e34         mrccs        14, 1, r2, cr9, cr4, {1}
      20:        30322033         eorscc        r2, r2, r3, lsr r0
      24:        35303531         ldrcc        r3, [r0, #-1329]!        ; 0xfffffacf
      28:        28203932         stmdacs        r0!, {r1, r4, r5, r8, fp, ip, sp}
      2c:        72657270         rsbvc        r7, r5, #112, 4
      30:        61656c65         cmnvs        r5, r5, ror #24
      34:        00296573         eoreq        r6, r9, r3, ror r5
    
    Disassembly of section .ARM.attributes:
    
    00000000 <.ARM.attributes>:
       0:        00003041         andeq        r3, r0, r1, asr #32
       4:        61656100         cmnvs        r5, r0, lsl #2
       8:        01006962         tsteq        r0, r2, ror #18
       c:        00000026         andeq        r0, r0, r6, lsr #32
      10:        726f4305         rsbvc        r4, pc, #335544320        ; 0x14000000
      14:        2d786574         cfldr64cs        mvdx6, [r8, #-464]!        ; 0xfffffe30
      18:        0600344d         streq        r3, [r0], -sp, asr #8
      1c:        094d070d         stmdbeq        sp, {r0, r2, r3, r8, r9, sl}^
      20:        14041202         strne        r1, [r4], #-514        ; 0xfffffdfe
      24:        17011501         strne        r1, [r1, -r1, lsl #10]
      28:        1a011803         bne        4603c <a+0x45ff8>
      2c:        22061e01         andcs        r1, r6, #1, 28
      30:        Address 0x0000000000000030 is out of bounds.



----------


## **qemu虛擬機計算逐步執行**

由qemu執行結果可以看出，在逐步執行過程中的記憶體分配方式，在進入到`**reset_handler(void)**`後，程式會預留**16 bits**的空間，首先，程式會將 `**b**` , `**c**` 放入到 `**0xf4**` ****和 `**0xf0**` 中，此時 `**0xec**`和 `**0xe8**`還是空的狀態。

![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553062201443_image.png)



在進入到`**add()**` 函式之前，會先額外分配16bit的空間，並且將 `**sp**` 指向的 `**0x200000e8**` 位置存放到 `**0xe4**` 的位置。在進入到function後，會在 `**0xdc**`和 `**0xd8**`的位置儲存`**a=2**`和 `**b=3**`的值，在加總完成後會先跳轉回去 `**lr**` 指向的位置，並且把`**a+b**`的數值儲存到`**0xec**` ****的位置，最後再將`d=7`的數值填入`0xe8`的位置。



![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553062447864_image.png)



**跳轉**
觀察進入function的方式，是透過`**bl 0x28**`去跳轉到`**0x28**`的位置。

![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063052224_image.png)


跳轉完成後，會讓`**sp**`指向`**0xe4**`的位置，並將`**0xd8**`儲存到`**r7**`，做為在function中的記憶體控制。

![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063588716_image.png)


`**r7**`的位置重新為`**0xd8**`

![r7改變前](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063653886_image.png)

![r7改變後1](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063678649_image.png)


`**r7**` 的位置被儲存到`**sp**`，代表程式重新開始使用在`**reset_handler(void)**`的記憶體空間。

![r7改變後2](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063747901_image.png)


跳轉前後的差異
`**bx lr**`之前，程式在`**0x42**`的位置。


![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063904555_image.png)


在`**bx lr**`執行完成後，程式會要回到`**0x1e**`的位置繼續執行`**reset_handler(void)**`的剩餘內容。

![](https://d2mxuefqeaa7sj.cloudfront.net/s_C8651D5AF17B581D702C75854A39A640311C6AE7BFF9B88677BE4DB0B45F01B2_1553063968082_image.png)


