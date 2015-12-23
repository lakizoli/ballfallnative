package com.ballfallnative;

import android.app.NativeActivity;
import android.app.ActionBar;
import android.os.Bundle;
import android.content.*;
import android.content.res.*;
import android.view.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.widget.*;
import android.util.*;
import com.google.android.gms.ads.*;

public class GameAcitivity extends NativeActivity {
	private LinearLayout _mainLayout;

	private PopupWindow _topLeftOverlay;
	private PopupWindow _topRightOverlay;
	private TextView _textTopRight;
	private TextView _textTopLeft;

	private PopupWindow _adOverlay;
	private AdView _adView;

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
		_adView = new AdView(this);
		_adView.setAdUnitId("ca-app-pub-5492739363747274/1590233145");
		_adView.setAdSize(AdSize.BANNER);

		AdRequest adRequest = new AdRequest.Builder()
				.addTestDevice("54F567A97CA221C8AF6DC24725DD98A9")
				.build();
        _adView.loadAd(adRequest);
	}

	@Override
	protected void onPause() {
		_adView.pause();
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		_adView.resume();
	}

	@Override
	protected void onDestroy() {
		_adView.destroy();
		super.onDestroy();
	}

	public void setTopLeftStyle(final float size, final float red, final float green, final float blue, final float alpha) {
		final GameAcitivity activity = this;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_adOverlay == null)
					_adOverlay = createOverlay (activity, 320, 50, 0, 0, _adView, Gravity.TOP, -5, -5, -5, -5);

				if (_topLeftOverlay == null)
					_topLeftOverlay = createOverlay (activity, 320, 50, 0, 50, _textTopLeft, Gravity.TOP | Gravity.LEFT, 5, 0, 5, 0);

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
					_topRightOverlay = createOverlay (activity, 320, 50, 0, 50, _textTopRight, Gravity.TOP | Gravity.RIGHT, 5, 0, 5, 0);

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

	private PopupWindow createOverlay (Context context, int width, int height, int offsetX, int offsetY, View overlayView, int gravity, int padLeft, int padTop, int padRight, int padBottom) {
		PopupWindow popUp = new PopupWindow(context);

		popUp.setWidth(width);
		popUp.setHeight(height);
		popUp.setWindowLayoutMode(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		popUp.setClippingEnabled(false);

		LinearLayout layout = new LinearLayout(context);
		layout.setOrientation(LinearLayout.VERTICAL);
		layout.setPadding(padLeft, padTop, padRight, padBottom);

		ViewGroup.MarginLayoutParams params = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		params.setMargins(0, 0, 0, 0);
		layout.addView(overlayView, params);

		popUp.setContentView(layout);

		popUp.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

		popUp.showAtLocation(_mainLayout, gravity, convertDpToPixel (offsetX), convertDpToPixel (offsetY));
		popUp.update();

		return popUp;
	}

	private int convertPixelsToDp(float px) {
		DisplayMetrics metrics = Resources.getSystem().getDisplayMetrics();
		float dp = px / (metrics.densityDpi / (float)DisplayMetrics.DENSITY_DEFAULT);
		return Math.round(dp);
	}

	private int convertDpToPixel(float dp) {
		DisplayMetrics metrics = Resources.getSystem().getDisplayMetrics();
		float px = dp * (metrics.densityDpi / (float)DisplayMetrics.DENSITY_DEFAULT);
		return Math.round(px);
	}
}
