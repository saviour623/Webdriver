
#           CHECK IF A MACRO IS PASSED AN EMPTY ARGUMENT

```Let V   -> __VA_ARGS__
   MAC_()  -> 0, 0 (changes to '0, ' later)
   MAC__() -> , 0  (used as a correction for MAC_)
```

##  APPROACH 1

The simple case is first handled by MAC1, where there is an
argument, or none at all:

1. `MAC_ V (V) -> MAC_(V)` if V is empty
2. else `MAC_ V (V) -> MAC_V(V)` (no expansion since no such macro)

In case 1, the end result is an expansion (0, 0, 1)
in case 2, it is (MAC_ 'V' (V), 1)
It's now easy to choose the second argument as the result
i.e either 0 or 1 for each respective cases.

### ISSUES
Some arguments like those below won't work by APPROACH 1.
1. Empty arguments like (,) and  (,,...)
2. Non Empty arguments like (N,), (, N...) and (N, N1...)
3. Only want *0* and *1* as the result for further Macro OP.

## APPROACH 2

- Cases likes (, N...) and (N, N1...) can be resolved if
we call MAC1 again on the result of its previous call.
This is because the ',', though preventing the expansion of
MAC_ V (V), actually seperates it to (MAC, N...(), 1) and
N...() as the second argument is selected. However, N...()
is not want we want (issue 3), so we call MAC1() again on
N...(), which then return what we expect, 1.

The same happens when the argument(s) is (,) or (N,)
[seperates to (MACR, (V), 1) or (MAC N, (V), 1)]
However, there is a problem. Calling MAC1 again when V
is empty will change its *0* to *1* since 0 is now passed to
the macro and *0* is obviously non-empty.
We simply change MAC_() to give us `0,` rather than `0, 0`
so that both the first and second MAC1 would give '' when
MAC_() is called i.e no argument was passed.
We define a new macro MAC2() that does the right thing later:
converts the '' to 0 right after the double call to MAC1.
All the STEP B process is wrapped in a macro, MAC3()

## Approach 3

Everything seems fine now, or maybe not. The following holds true:

```
()        -> 0,
(,)       -> 0(V) (unwanted)
(N,)      -> 0(V) (unwanted)
(N)       -> 1
(N, N1..) -> 1

```
To behave like C++/C23 __VA_OPT__, we need to handle the (,) and (N,) cases. However, we desire (,) to return 0 instead.
As seen above, passing a single ',' or 'N,' expands to 0(), therefore we need a way to check if for (x, ), 'x' is empty. This is easy: we can define a macro `MAC0` like this `MAC0(V) CHECK_FIRST(V)` which expands to *0* or *1*. Since our previous macros always chooses the secong argument, we could add a '0,' before the CHECK_FIRST

```C
#define MAC0(...) 0, CHECK_FIRST(V)
```
And now all we have to do is to define a macro that concatenate 'MAC' with our previous result from MAC3(), choosing only the second argument. We wrap this in a new macro `MAC4`.

```
MAC4:

(MAC ## 1, 1)    -> 1
(MAC ## 0(V), 1) -> (0, CHECK_FIRST(V), 1)
-> (0, 0, 1) -> 0
or (0, 1, 1) -> 1
(MAC ## 0, 1)    -> 1
```

The last, is a problem. Empty argument now returns 1, since the 'MAC ## 0' cannot expand without a '()', allowing 1 to be selected. To solve this, we add a dummy CHECK_FIRST to MAC0. So now we have MAC0 as:

```C
#define MAC0(...) 0, CHECK_FIRST(V), CHECK_FIRST
/* and we can call it as */
MAC0(V)(V)
/* In each CHECK_FIRST, only the first argument of (V) is needed */
```

We define our last macro that does the final check and selection `MAC5`. We can now do the check like this:

```
MAC5:

(MAC ## 1(FIRST), 1)  -> (MAC1(FIRST), 1)       -> 1
(MAC ## 0(FIRST), 1)  -> (0, 0, CHECK_FIRST, 1) -> 0
(MAC ## 0(...)(FIRST) -> (0, CHECK_FIRST(V), CHECK_FIRST(FIRST)
-> CHECK_FIRST(V)         -> 0 or 1
```

## STEP 4

V is always wrapped with multiple parenthesis when the
arguments are (,) or (N,). This hinders the last check by MAC0
to evaluate correctly. This is as a result of `(,)` and `(N, )` breaking `MAC_ V (V)` to `(V)` at each call of MAC1 or MAC2. So calling `MAC1(MAC1(V))` would produce ((V)) and MAC2 on the later, would produce `0(((V)))` since MAC0 requires only a single (). We could unroll the parenthesis as many times, using a macro that expands parenthesis, leaving only the inner values. Infact we do this along the process, rather than at the end.

We use a PAREN macro, we at the first call to MAC1 with an argument such as '(, )'. It does nothing since (PAREN(V)) is (V). On the second call, V is now (V) and (PAREN( (V) )) still remains (V) because PAREN() always expands its parenthesized argument, in this case (V) to V.
The same procedure continues for MAC2, so that at the end, we only have one (V) and can correctly do MAC ## 0(V).

## ADDITION

The compiler complains about passing an empty parameter to a macro, we define a dummy macro DD_ that is passed along with V to fill in cases where V is empty (has no side effect)


