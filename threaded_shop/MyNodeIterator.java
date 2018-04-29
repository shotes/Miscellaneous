//Daniel Ehrlich


import java.util.NoSuchElementException;
import java.util.Iterator;

public class MyNodeIterator<E> implements Iterator<E>{
	private MyNode<E> node;
	public MyNodeIterator(MyNode<E> n){
		node = n;
	}
	
	public boolean hasNext(){
		return node != null;
	}
	
	public E next(){
		if(hasNext()){
			E value = node.getV();
			node = node.getNext();
			return value;
		}
		throw new NoSuchElementException();
	}
}
