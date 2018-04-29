//Daniel Ehrlich


import java.util.Arrays;

public class Test2{
	public static void main(String args[]){
		MyList<Integer> empty_list = new MyList<Integer>();
		MyList<Integer> list = new MyList<Integer>(Arrays.asList(1, 2, 3, 4, 5, 6));
		MyList<Integer> list2 = new MyList<Integer>(list.clone());
		System.out.println(empty_list);
		System.out.println(empty_list.reverse());
		System.out.println(list);
		System.out.println(list.pop());
		System.out.println(list.reverse());
		list.push(11);
		list.push(12);
		System.out.println(list);
		System.out.println(list.peek());
		System.out.println(list);
		System.out.println(list2);
		int sum = 0;
		for (int e : list) { sum += e; }
		System.out.println(sum);
	}
}
