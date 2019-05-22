#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <cmath>
ILOSTLBEGIN

const int Veg_Beg = 1;
const int Veg_End = 2;
const int Nvg_Beg = 3;
const int Nvg_End = 5;
const int Oil_Beg = 1;
const int Oil_End = 5;
const int Month_Beg = 1;
const int Month_End = 6;
const int XMonth_Beg = 0;
const int XMonth_End = 6;

const int NOR_GATE = -1;
const int ZERO_INPUT = 0;
const int NOT_VALID = -2;
const int MAX_DEPTH = 6;

int depth;
int max_nodes = int(pow(2, depth)) - 1;
int max_nors = int(pow(2, depth + 1)) - 1;

int* initializeInputs(int inputs[], int size, int num_inputs)
{
  int value = 0;
  int permutation = int(pow(2, num_inputs - 1));

  // Initialize with zeros
  for (int i = 0; i < (2 ^ num_inputs) + 1; i++)
  {
    inputs[i] = value;
  }
  for (int i = 2 ^ num_inputs + 1; i < size; i++)
  {
    if (i % permutation == 0)
    {
      if (value == 0)
        value = 1;
      else
        value = 0;
    }
    if (i % int(pow(2, num_inputs)) == 0 and i != int(pow(2, num_inputs)))
    {
      permutation /= 2;
    }
    inputs[i] = value;
  }
  return inputs;
}

IloNumVarArray SOLUTION;
IloNumVarArray IS_NOR;
IloNumVar solution(int i) { return SOLUTION[i]; }
IloNumVar isNor(int i) { return IS_NOR[i]; }
IloNumVar leftChild(int i) { return SOLUTION[2 * i + 1]; }
IloNumVar rightChild(int i) { return SOLUTION[2 * i + 2]; }

/**
 * Perform NOR operation between two nodes.
 */
// IloNumArray* norOperation(IloNumVarArray result, IloNumVarArray left, IloNumVarArray right)
// {
//   for (int i = 0; i < result.getSize(); ++i)
//   {
//     result[i] = (left[i] + right[i]);
//   }
//   return result;
// }

// void printSolution(int solution[], int index){
//   cout << index+1 << " ";
//   if(solution[index] == -1){
//     cout << -1 << " ";
//     int left = 2*index+1;
//     cout << left +1 << " ";
//     int right = 2*index+2;
//     cout << right +1 << endl;
//     printSolution(solution, left);
//     printSolution(solution, right);
//   } else {
//     cout << solution[index] << " " << 0 << " " << 0 << endl;
//   }
// }

int main()
{

  // INPUTS AND INITIALIZING VARIABLES

  int num_inputs, u;
  cin >> num_inputs;
  int original[int(pow(2, num_inputs))];

  for (int k = 0; k < int(pow(2, num_inputs)); ++k)
  {
    cin >> u;
    original[k] = u;
  }

  int size = (num_inputs + 1) * int(pow(2, num_inputs));
  int a[size];

  int* inputs = initializeInputs(a, size, num_inputs);

  // CONSTRAINTS AND ALL THOSE THINGS

  IloEnv env;
  for (int i = 0; i < MAX_DEPTH; ++i)
  {
    IloModel model(env);

    // NumVarArrayInitialization

    SOLUTION = IloNumVarArray(env, max_nodes, -1, num_inputs);
    IS_NOR = IloNumVarArray(env, max_nodes, 0, 1, ILOBOOL);

    for (int i = 0; i < max_nodes; ++i)
    {
      // We impose that if sol[i] == -1 --> is_nor[i] == 1
      model.add(isNor(i) - 1 >= -1 * (-1 - solution(i)));
      model.add(isNor(i) - 1 <= 0);

      // We impose that if isNor(i) == 0 <--> rightChild(i) == 0
      // Upper bound of rightChild(i) - 0: max_nodes.
      // Lower bound of rightChild(i) - 0: -1.

      // Right implication
      model.add(rightChild(i) >= (-1) * isNor(i));
      model.add(rightChild(i) <= max_nodes * isNor(i));

      // Left implication


      // And in the same way if isNor(i) == 0 <--> leftChild(i) == 0

      // Right impication
      model.add(leftChild(i) >= (-1) * isNor(i));
      model.add(leftChild(i) <= max_nodes * isNor(i));

      // Left implication
    }

    // for (int m = Month_Beg; m <= Month_End; ++m)
    // {
    //   IloExpr expr1(env);
    //   for (int o = Oil_Beg; o <= Oil_End; ++o)
    //     expr1 += u(o, m);
    //   model.add(expr1 == p(m));
    //   expr1.end();

    //   IloExpr expr2(env);
    //   for (int o = Oil_Beg; o <= Oil_End; ++o)
    //     expr2 += h[o] * u(o, m);
    //   model.add(expr2 <= hub * p(m));
    //   model.add(expr2 >= hlb * p(m));
    //   expr2.end();

    //   IloExpr expr3(env);
    //   for (int o = Veg_Beg; o <= Veg_End; ++o)
    //     expr3 += u(o, m);
    //   model.add(expr3 <= uup[Veg_Beg]);
    //   expr3.end();

    //   IloExpr expr4(env);
    //   for (int o = Nvg_Beg; o <= Nvg_End; ++o)
    //     expr4 += u(o, m);
    //   model.add(expr4 <= uup[Nvg_Beg]);
    //   expr4.end();
    // }

    // Objective function

    IloExpr obj(env);

    for (int m = 0; m < max_nodes; ++m)
    {
      obj += isNor(m);
    }

    model.add(IloMinimize(env, obj));
    obj.end();

    IloCplex cplex(model);
    cplex.solve();
    cout << cplex.getObjValue() << endl;

    // OUTPUT OF THE SOLUTION

    IloNumArray v(env);
    cplex.getValues(v, SOLUTION);

    // printSolution(cplex, v, 0);
  }
  env.end();
}
