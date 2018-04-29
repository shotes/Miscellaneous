//Daniel Ehrlich


import java.util.Iterator;

public final class MyNode<E> implements Iterable<E>{
	private final E v;
	private MyNode<E> next;
	public MyNode(E val, MyNode<E> node){
		v = val;
		next = node;
	}
	public E getV(){
		return v;
	}
	public MyNode<E> getNext(){
		return next;
	}
	public void setNext(MyNode<E> n){
		next = n;
	}
	public MyNodeIterator<E> iterator(){
		return new MyNodeIterator<E>(this);
	}
}
