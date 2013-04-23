// test.java


/*
// this one works in the console
class test 
{
	static 
	{
		System.loadLibrary( "Debug/HelloJavaDLL" );
	}

	private native void print();

	public static void main( String[] args )
	{
		new test().print();
	}
}
*/

import java.applet.*;
import java.awt.*;

public class test extends Applet 
{
	static 
	{
		System.loadLibrary( "Debug/HelloJavaDLL" );
	}

	private native void print();

	public void init() 
	{
		//new test().print();
	}

	public void stop()
	{
	}
	
	public void paint(Graphics g) 
	{
		g.drawString( "Hello world from java 4!", 25, 50 );
	}
}











/*
import java.applet.*;
import java.awt.*;

import java.security.AccessController;
import java.security.PrivilegedAction;

public class test extends Applet 
{
	private static native void myout();

	
	public static void main( String[] args )
	{
		new test().print();
	}
	
	
	static 
	{
		//System.loadLibrary( "Debug/HelloJavaDLL" );
		//System.load( "Debug/HelloJavaDLL.dll" );
		try{
			AccessController.doPrivileged(new PrivilegedAction()
			{
				public Object run()
			{
				try
				{
							// privileged code goes here, for example:
				 //System.loadLibrary("Debug/HelloJavaDLL");
				 //System.load("C:/Projects/sigmaOsg/osghelp/HelloJavaDLL/Debug/HelloJavaDLL.dll");
				 System.out.println("DLL IS LOADED");

				 } 
				catch (Exception e)
				 {
					 System.out.println("Unable to load Mydll 1");
				 }
			}
			 });
		} 
			catch (Exception e)
		{
				System.out.println("Unable to load Mydll 2");
		}

		myout();
	}
	
	public void init() 
	{
		//resize(150,25);
		
    }

	public void paint(Graphics g) 
	{
		g.drawString("Hello world from java 4!", 25, 50);
	}
}
*/