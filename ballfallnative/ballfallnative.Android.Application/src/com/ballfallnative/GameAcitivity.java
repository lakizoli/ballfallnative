package com.ballfallnative;

import android.app.NativeActivity;
import android.app.ActionBar;
import android.os.Bundle;
import android.os.Build;
import java.util.concurrent.locks.*;
//import java.util.concurrent.atomic.AtomicInteger;
import java.util.*;
import android.content.*;
import android.content.res.*;
import android.view.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.widget.*;
import android.util.*;
import android.net.*;
import android.media.*;
import java.io.*;
import com.google.android.gms.ads.*;

public class GameAcitivity extends NativeActivity {
	private LinearLayout _mainLayout;

	private PopupWindow _topLeftOverlay;
	private PopupWindow _topRightOverlay;
	private StrokedTextView _textTopRight;
	private StrokedTextView _textTopLeft;

	private PopupWindow _adOverlay;
	private AdView _adView;

	private SoundPlayer _soundPlayer;

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
		_textTopRight = new StrokedTextView(this);
		_textTopRight.setText("");

		_textTopLeft = new StrokedTextView(this);
		_textTopLeft.setText("");

		Typeface tf = Typeface.createFromAsset(getAssets(), "font.ttf");
		_textTopRight.setProperties (tf, new byte[]{ (byte)255, (byte)92, (byte)16, (byte)16 }, convertDpToPixel (5), new byte[]{ (byte)255, (byte)255, 0, 0 });
		_textTopLeft.setProperties (tf, new byte[]{ (byte)255, (byte)92, (byte)16, (byte)16 }, convertDpToPixel (5), new byte[]{ (byte)255, (byte)255, 0, 0 });

		//Setup adMob overlay
		_adView = new AdView(this);
		_adView.setAdUnitId("ca-app-pub-5492739363747274/1590233145");
		_adView.setAdSize(AdSize.BANNER);

		AdRequest adRequest = new AdRequest.Builder()
				.addTestDevice("54F567A97CA221C8AF6DC24725DD98A9")
				.build();
        _adView.loadAd(adRequest);

		//Setup sounds and music
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		_soundPlayer = new SoundPlayer();
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

	public void initAdMob () {
		final GameAcitivity activity = this;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_adOverlay == null)
					_adOverlay = createOverlay (activity, 320, 50, 0, 0, _adView, Gravity.TOP, -5, -5, -5, -5);
			}
		});
	}

	public void setTopLeftStyle(final float size, final float red, final float green, final float blue, final float alpha) {
		final GameAcitivity activity = this;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_topLeftOverlay == null)
					_topLeftOverlay = createOverlay (activity, 320, 50, 0, 55, _textTopLeft, Gravity.TOP | Gravity.LEFT, 5, 0, 5, 0);

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
					_topRightOverlay = createOverlay (activity, 320, 50, 0, 55, _textTopRight, Gravity.TOP | Gravity.RIGHT, 5, 0, 5, 0);

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

	private native static void soundCompleted (int soundID);

	public int playSound (final String asset, final boolean looped) {
		_soundPlayer.WaitLoadAll();
		_soundPlayer.playSound (asset, looped);

//		final GameAcitivity activity = this;
//		final String assetFile = asset;
//		final boolean needToLoop = looped;
//		final int soundID = _soundID.incrementAndGet ();
//
//		runOnUiThread(new Runnable() {
//			@Override
//			public void run() {
//				try {
//					AssetFileDescriptor assetFileDescriptor = activity.getAssets ().openFd (assetFile);
//					Log.d ("sound", "assetFile: " + assetFile);
//
//					MediaPlayer player = new MediaPlayer();
//					_mediaPlayers.put (soundID, player);
//
//					player.setDataSource(assetFileDescriptor.getFileDescriptor ());
//					player.setVolume(1.0f, 1.0f);
//					if (needToLoop)
//						player.setLooping(true);
//					player.setOnCompletionListener(new android.media.MediaPlayer.OnCompletionListener() {
//						@Override
//						public void onCompletion(MediaPlayer player) {
//							GameAcitivity.soundCompleted (soundID);
//							_mediaPlayers.remove (soundID);
//						}
//					});
//					player.prepare();
//					player.start();
//				} catch (IOException ex) {
//					ex.printStackTrace ();
//				}
//			}
//		});
//
//		return soundID;
		return 0;
	}

	private class SoundPlayer {
		private AudioManager audioManager;
		private SoundPool soundPool;
		private float actVolume, maxVolume, volume;

		private String[] sounds = new String[] {"whistleblower.ogg", "click.ogg"};
		private TreeMap<Integer, String> soundNames = new TreeMap<Integer, String> ();
		private TreeMap<String, Integer> soundIDs = new TreeMap<String, Integer> ();
		private volatile boolean isAllLoaded = false;
		private final Lock lock = new ReentrantLock();

		public SoundPlayer () {
			isAllLoaded = false;

			// AudioManager audio settings for adjusting the volume
			audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
			actVolume = (float) audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
			maxVolume = (float) audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
			volume = actVolume / maxVolume;

			// Load the sounds
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
				AudioAttributes attributes = new AudioAttributes.Builder()
					.setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
					.setFlags(AudioAttributes.FLAG_AUDIBILITY_ENFORCED)
					.setUsage(AudioAttributes.USAGE_GAME)
					.build();
				soundPool = new SoundPool.Builder().setAudioAttributes(attributes).setMaxStreams(10).build();
			} else {
				soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
			}
			soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
				@Override
				public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
					if (status == 0) {
						lock.lock ();
						try {
							soundIDs.put (soundNames.get (sampleId), sampleId);
						} finally {
							lock.unlock ();
						}
					}
				}
			});

			try {
				for (String sound : sounds) {
					AssetFileDescriptor assetFileDescriptor = getAssets ().openFd (sound);
					String soundName = sound.substring(0, sound.lastIndexOf('.'));
					soundNames.put (soundPool.load(assetFileDescriptor, 1), soundName);
					assetFileDescriptor.close();
				}
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

		public void WaitLoadAll () {
			try {
				if (isAllLoaded)
					return;

				for (String sound : sounds) {
					String soundName = sound.substring(0, sound.lastIndexOf('.'));
					while (getSoundID (soundName) == null) {
						Thread.sleep(100);
					}
				}

				isAllLoaded = true;
			} catch (InterruptedException ex) {
				ex.printStackTrace ();
			}
		}

		public Integer getSoundID (String name) {
			lock.lock ();
			try {
				return soundIDs.get (name);
			} finally {
				lock.unlock();
			}
		}

		public void playSound (String name, boolean looped) {
			Integer soundID = soundIDs.get(name);
			if (soundID != null) {
				soundPool.play(soundID, volume, volume, 1, 0, 1f);
			}
		}
	}

	public String readFile (String fileName) {
		String content = "";
		FileInputStream inputStream = null;
		try {
			inputStream = openFileInput (fileName);

			StringBuffer fileContent = new StringBuffer("");
			byte[] buffer = new byte[1024];

			int n = 0;
			while ((n = inputStream.read(buffer)) != -1) { 
				fileContent.append(new String(buffer, 0, n)); 
			}

			content = fileContent.toString ();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (inputStream != null)
					inputStream.close ();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		return content;
	}

	public void writeFile (String fileName, String content) {
		FileOutputStream outputStream = null;
		try {
			outputStream = openFileOutput(fileName, Context.MODE_PRIVATE);
			outputStream.write(content.getBytes());
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (outputStream != null)
					outputStream.close ();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
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
