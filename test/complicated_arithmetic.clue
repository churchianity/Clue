
` this is an intentionally over-convuluted set of arithmetic statements & assignments to test the robustness of the parser
` @TODO it should ideally use every operator that either does arithmetic, logic, or assignment
x : number = 5 ^ 3 + 4 * (2 ** -2 / 2);
y := ((((4 - 2 / (66 % 33) + -123) | ~6) ** 1) - 1);
z := x - (!0 / ~2.12);

x + y + z;

