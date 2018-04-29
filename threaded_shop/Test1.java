//Daniel Ehrlich


import java.util.Iterator;
import java.util.function.*;

public class Test1{
	public static void main(String args[]){
		MyNode<Integer> intlist = new MyNode<Integer>(1,
									new MyNode<Integer>(2,
									new MyNode<Integer>(3,
									new MyNode<Integer>(4,
									new MyNode<Integer>(5,null)))));
		MyNode<Double> doublelist = new MyNode<Double>(1.1,
									new MyNode<Double>(2.2,
									new MyNode<Double>(3.3,
									new MyNode<Double>(4.4,
									new MyNode<Double>(5.5,null)))));
		print(intlist);
		int sum1 = int_sum(intlist);
		int sum2 = (int)num_sum(intlist);
		System.out.println("intlist int_sum " + sum1 + " == intlist num_sum " + sum2);
		print(doublelist);
		double dsum = num_sum(doublelist);
		System.out.println("doublelist sum: " + dsum);
		
	}
	public static int int_sum(MyNode<Integer> n){
		int sum = 0;
		for(Integer x : n){
			sum += x;
		}
		return sum;
	}
	public static double num_sum(MyNode<? extends Number> n){
		double sum = 0;
		for(Number x : n){
			sum += x.doubleValue();
		}
		return sum;
	}
	public static void print(MyNode<?> n){
		System.out.println("List of " + n.getV().getClass().getName());
		MyNodeIterator<?> iter = n.iterator();
		for(Object x : n){
			System.out.println(x);
		}
	}
}
