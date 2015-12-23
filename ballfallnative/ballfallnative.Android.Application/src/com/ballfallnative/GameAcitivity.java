package com.ballfallnative;

import android.app.NativeActivity;
import android.app.ActionBar;
import android.os.Bundle;
import android.content.*;
import android.view.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.widget.*;
import android.util.*;
//import com.google.android.gms.ads.*;

public class GameAcitivity extends NativeActivity {
	private LinearLayout _mainLayout;
	private PopupWindow _topLeftOverlay;
	private PopupWindow _topRightOverlay;
	private TextView _textTopRight;
	private TextView _textTopLeft;

	//private AdView _adView;

	static {
		System.loadLibrary("ballfallnative");
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		//Hide status bar
		View decorView = getWindow().getDecorView();
		decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);

		// Remember that you should never show the action bar if the
		// status bar is hidden, so hide that too if necessary.
		ActionBar actionBar = getActionBar();
		actionBar.hide();

		//Create the main linear layout for overlays
		_mainLayout = new LinearLayout(this);

		ViewGroup.MarginLayoutParams params = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		params.setMargins(0, 0, 0, 0);

		setContentView(_mainLayout, params);

		//Setup game overlays
		_textTopRight = new TextView(this);
		_textTopRight.setText("");

		_textTopLeft = new TextView(this);
		_textTopLeft.setText("");

		Typeface tf = Typeface.createFromAsset(getAssets(), "font.ttf");
		_textTopRight.setTypeface(tf, Typeface.NORMAL);
		_textTopLeft.setTypeface(tf, Typeface.NORMAL);

		//Setup adMob overlay
		//_adView = new AdView(this);
		//AdRequest adRequest = new AdRequest.Builder().build();
        //_adView.loadAd(adRequest);
	}

	public void setTopLeftStyle(final float size, final float red, final float green, final float blue, final float alpha) {
		final GameAcitivity activity = this;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_topLeftOverlay == null)
					_topLeftOverlay = createOverlay (activity, 320, 50, _textTopLeft, Gravity.TOP | Gravity.LEFT);

				_textTopLeft.setTextSize(TypedValue.COMPLEX_UNIT_DIP, size);

				byte redByte = (byte) (255.0 * red);
				byte greenByte = (byte) (255.0 * green);
				byte blueByte = (byte) (255.0 * blue);
				byte alphaByte = (byte) (255.0 * alpha);
				_textTopLeft.setTextColor(Color.argb(alphaByte, redByte, greenByte, blueByte));
			}
		});
	}

	public void setTopRightStyle(final float size, final float red, final float green, final float blue, final float alpha) {
		final GameAcitivity activity = this;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_topRightOverlay == null)
					_topRightOverlay = createOverlay (activity, 320, 50, _textTopRight, Gravity.TOP | Gravity.RIGHT);

				_textTopRight.setTextSize(TypedValue.COMPLEX_UNIT_DIP, size);

				byte redByte = (byte) (255.0 * red);
				byte greenByte = (byte) (255.0 * green);
				byte blueByte = (byte) (255.0 * blue);
				byte alphaByte = (byte) (255.0 * alpha);
				_textTopRight.setTextColor(Color.argb(alphaByte, redByte, greenByte, blueByte));
			}
		});
	}

	public void setTopLeftStatus(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				_textTopLeft.setText (text);
			}
		});
	}

	public void setTopRightStatus(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				_textTopRight.setText (text);
			}
		});
	}

	private PopupWindow createOverlay (Context context, int width, int height, TextView overlayView, int gravity) {
		PopupWindow popUp = new PopupWindow(context);

		popUp.setWidth(width);
		popUp.setHeight(height);
		popUp.setWindowLayoutMode(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		popUp.setClippingEnabled(false);

		LinearLayout layout = new LinearLayout(context);
		layout.setOrientation(LinearLayout.VERTICAL);
		layout.setPadding(5, 0, 5, 0);

		ViewGroup.MarginLayoutParams params = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		params.setMargins(0, 0, 0, 0);
		layout.addView(overlayView, params);

		popUp.setContentView(layout);

		popUp.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

		popUp.showAtLocation(_mainLayout, gravity, 0, 0);
		popUp.update();

		return popUp;
	}
}
