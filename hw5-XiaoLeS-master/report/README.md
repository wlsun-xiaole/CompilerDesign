# 作業 5 報告

> 學生：孫汶琳
>
> 學號：0680707

## Bonus
Spec上的：
- Code generation for boolean types
- Code generation for array types
- Code generation for string types
- Code generation for real types

額外的Real & Integer處理，包含：
- BinaryOperator兩邊type不同時的數學運算和邏輯運算
- Assignment兩邊type不同時的處理

## 作業 5 心得
寫完了好開心啊！

雖然助教在影片中說這次作業比較簡單，但是我覺得一開始很難啊ＱＱ
最困難的就是在一開始要找出不同種類的Node之間的合作關係，像我跟助教寫的code感覺就會差很多（因為產生的assembly不太一樣）。最一開始的variable和constant我就有重寫過，第一次寫的雖然測資會過，但是接下來遇到Expression等等就會很難動，還好測資的順序很棒，我基本上是照著測資順序去實做的，自從重寫後，我就會開始去思考這個Node和其他Node可能的合作關係（其實就是不斷的push,pop stack），再來決定Code Generation的點，所以後面實做起來真的就越來越順了！不過偶爾還是會因為ＶＰ而苦惱，因為一開始會很想把所有的code都擠在Generator（我直接寫在SementicAnalyzer裡）上，而不想到visitChild裡面去做，後來就放棄了...因為真的有些code需要在兩個child之間產生，像是if, while等等，這時我就在思考這樣會不會就破壞了ＶＰ的結構了呢？接下來主要的困難點是在Bonus的部分在call readReal的時候出了從未見過的error message，我真的測到完全不知道怎麼辦（有點崩潰），真的非常感謝助教很神奇的找到了原因，如果那位助教看到了不知道可不可以跟我說一下怎麼發現的ＸＤ？（揮手）另外還有一個比較小的難關是在遇到string和real constant時的程式片段不能隨便放，也就是不能在像是main裡面插入，我最直覺的想法是file operation但是有點麻煩，再來就是traverse兩次（寫到後面遇到這問題會完全不考慮這個做法），雖然後來助教回覆說可以先存起來放到最後面再輸出就好，不過因為看到人家riscv的compiler是放在前面的，我就很想放在前面，所以就稍微有點像是在複習計概一樣研究了一下file operation，還用到了我以前沒用過的ftell()等，算是小小的收穫吧！

另外一個讓我一開始很痛苦的原因，就是我一開始會想要產生跟助教範例一模一樣的code，寫到後來比較熟悉之後，像是expression, condition相關的部分，我就是大概看一下助教的範例，了解大概有哪些指令會用到，還有可能的操作方法等，然後再思考以我的架構要怎麼來時做比較好，或者說，以我自己寫assembly的邏輯怎麼樣產生code。用自己寫assembly的邏輯去寫有個好處，就是debug看assembly很快，只要跟自己想像中的不一樣基本上就很好找問題出在哪了。不過助教的範例code真的對一開始相當有幫助，當然還有sample code，如果沒有sample code我大概前面幾次就會放棄作業了（認真），這次能夠順利寫完，完全都是因為助教的sample code寫的太好了，基本上就是工具都擺在眼前，拿了就可以直接用的超理想狀態，每個Node甚至symbol table的相關method都很完整，我也因此見識到了什麼叫做C++，這真的可以說是我覺得這門課最大的收穫。或許，以後我的人生不一定會繼續開發compiler，但是肯定會有C++的陪伴，所以能在在學期間見識到會寫C++的人寫的code真的...做夢都會笑ＸＤ

感謝助教們真的很用心的為大家解決問題，研究所的課因為人數比較少，助教費通常低得可憐，但是助教們還是那麼用心的為大家不僅是回答問題，還有設計題目，架設平台、環境等，真的是很令人感動！雖然我個人能力不足，好像也不能幫什麼忙，不過我希望在打完報告到把板子還回去的這兩天內可以幫忙研究一點Mac上板子的使用，還有像是上次報告中提到的windows上的wsl2+docker的部分（測試過了，看起來可以用），希望大家以後能輕鬆一點XD

## 整學期編譯器作業心得
第一次報告裡面講過，其實這算是我第二次修這門課，上一次修課時大概在期中考之後就退選了，主要是我完全就不知道自己在修什麼，當然很大的一個原因是我自己選太多課，不過因為真的很想把這門課修好，所以毅然決然的就退選了。過了大概兩年吧，又跑回來修，這次我花比較多心思在這堂課上面，又剛好遇到疫情而採線上教學，而老師也願意提供錄影，讓我可以重複複習沒聽清楚的部分，這對我而言這幫助很大，因為上課的全程要能保持專注對我來說有點困難，也不是整學期都能很順利的每週馬上複習，會很容易就跟不上進度，這時候錄影就是大大的助益！當然，對期中、期末考的複習也是幫助很大ＸＤ

還有一個讓我很慶幸自己重修的點前面其實講過了，就是助教的sample code，另外還有Github、docker、自動評測的使用。其實Google一下就能找到很多人把修課的code放在github上，如果是上一次修課的我的話，肯定會抱著那些殘破不堪的code來參考，然後寫出一個不知道會不會動，但是肯定不想再看到他那種的code，印象中這次的作業（第三次作業開始？）要求就不太一樣了，而且這次我理論的部分也學得比較認真，再加上要用VP，以及助教有給令我小鹿亂撞(?)的starter code了，沒有理由去看那些code。還有就是Github能夠直接繳交作業真的很棒，不用擔心自己壓縮檔會不會少包什麼啊，還是檔名弄錯啊等等無聊的問題，再加上docker的使用，讓環境設定變得超級簡單（當然是在助教的努力下才能有這樣的便利<(_ _)>）。最後，也是最最最重要的一點是自動評測，這是我在兩次修課中覺得最為明顯的差異，上次沒有自動評測我完全不知道自己到底寫得怎樣，或者有沒有少寫什麼，就算真的少了什麼，也是要到結果出來才知道（或者說不一定會知道），然後還沒研究完就會開始下一次的作業，所以也不會去重新審視、思考自己前面有什麼概念不清楚或者有錯，反而會直接拿助教的sample code來開始寫好一次的作業，也開啟了惡性循環模式，當然，成績和學習成效也都不會太好；反觀這次有了自動評測之後，我可以在寫作業的時候就知道自己還有哪裡概念不清楚或者需要補強的地方，畢竟大部分的人看到成績（結果）之後就懶了，真正最有效能學習的時刻就是在寫作業的時候，所以能夠即時測試自己的程式真的是讓我這次能夠學得比上次好很多的關鍵！

最後，不免俗的，但也真心的感謝老師和助教們用心的課程規劃，很久沒修到這麼扎實又有收穫的課程了，希望平行程式設計也能夠效仿這個模式（喂）。
