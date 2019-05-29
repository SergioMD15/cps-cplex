#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <cmath>
ILOSTLBEGIN

int max_nodes;
int max_nors;

int *initializeInputs(int inputs[], int size, int num_inputs)
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
IloNumVarArray INPUTS_ASSOCIATION;
IloNumVar solution(int i) { return SOLUTION[i]; }
IloNumVar isNor(int i) { return IS_NOR[i]; }
IloNumVar leftChild(int i){ return SOLUTION[2 * i + 1]; }
IloNumVar rightChild(int i) { return SOLUTION[2 * i + 2]; }
IloNumVar associated_input(int i, int j) { return INPUTS_ASSOCIATION[i + j]; }

void printSolution(IloCplex cplex, IloNumArray solution, int index){
  cout << index + 1 << " ";
  if(solution[index] == -1){
    cout << -1 << " ";
    int left = 2 * index + 1;
    cout << left + 1 << " ";
    int right = 2 * index + 2;
    printSolution(cplex, solution, left);
    printSolution(cplex, solution, right);
  } else {
    cout << solution[index] << " " << 0 << " " << 0 << endl;
  }
}

void printArray(IloNumArray array){
  for(int i = 0; i< array.getSize(); ++i){
    cout << array[i] << " ";
  }
}

int main()
{

  const int MAX_DEPTH = 4;

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

  int *inputs = initializeInputs(a, size, num_inputs);
  int lower;
  int upper;

  // CONSTRAINTS

  for (int j = 0; j < MAX_DEPTH; ++j)
  {
    IloEnv env;
    IloModel model(env);

    max_nodes = int(pow(2, j + 1)) - 1;
    max_nors = int(pow(2, j)) - 1;
    int input_num = pow(2, num_inputs);

    // NumVarArrayInitialization

    SOLUTION = IloNumVarArray(env, max_nodes, -1, num_inputs, ILOINT);
    IS_NOR = IloNumVarArray(env, max_nodes, 0, 1, ILOBOOL);
    INPUTS_ASSOCIATION = IloNumVarArray(env, max_nodes * input_num, 0, 1, ILOBOOL);

    for (int i = 0; i < max_nodes; ++i)
    {
      // The leaves of the tree must be != -1
      for(int k = max_nors - 1; k < max_nodes; ++k){
        model.add(isNor(i) == 0);
      }

      // We impose that sol[i] = j --> associated_input(i,k) = inputs[j + k]
      for(int j = 0; j < num_inputs; j++){
        for(int k = 0; k < int(pow(2,num_inputs)); k++){
          // For the inputs

          // For NOR gates
          if(2 * i + 1 < max_nodes){
            int left_input = input_num * (2 * i + 1);
            int right_input = input_num * (2 * i + 2); 

            model.add(associated_input(left_input,k))
                       + associated_input(right_input,k)
                        <= 2 * (1 - associated_input(i,k));

            model.add(associated_input(left_input,k))
                       + associated_input(right_input,k) - 1
                        >= -associated_input(i,k));
            // model.add(associated_input(left_input,k)
            //           + associated_input(right_input,k)
            //             + 2 * associated_input(i,k) - 2 <= 0);

            // model.add(associated_input(left_input,k)
            //           + associated_input(right_input,k)
            //             + associated_input(i,k) >= (1 - isNor(i)));
          }
        }
      }

      // We impose that if is_nor[i] == 1 <--> sol[i] == -1
      // Upper bound of sol[i] + 1: num_inputs + 1
      // Lower bound of sol[i] + 1: 0
      // Right implication
      model.add(solution(i) + 1 <= (num_inputs + 1)*(1 - isNor(i)));
      model.add(solution(i) + 1 >= 0);
      // Left implication
      model.add(solution(i) >= (-isNor(i)));

      if(2 * i + 1 < max_nodes){
        // We impose that if isNor(i) == 0 --> rightChild(i) == 0
        // Upper bound of rightChild(i) - 0: num_inputs.
        // Lower bound of rightChild(i) - 0: -1.

        // Right implication
        model.add(rightChild(i) >= (-1 * isNor(i)));
        model.add(rightChild(i) <= (num_inputs * isNor(i)));

        // And in the same way if isNor(i) == 0 --> leftChild(i) == 0
        // Upper bound of leftChild(i) - 0: num_inputs.
        // Lower bound of leftChild(i) - 0: -1.

        // Right impication
        model.add(leftChild(i) >= (-1 * isNor(i)));
        model.add(leftChild(i) <= num_inputs * isNor(i));
      }
    }
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

    printSolution(cplex, v, 0);
    env.end();
  }
}