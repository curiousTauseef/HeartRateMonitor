package com.shnayder.heartratemonitor;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

public class HeartRateMonitor extends Activity {
	public static String HRM_TAG = "Heart Rate Monitor Java";

	private Camera _camera = null;
	private ImageView _imageView = null;
	private HeartRateMonitorPreview _cameraPreview = null;

	@SuppressWarnings("unused")
	private boolean checkCamera(final Context context) {
		if (!context.getPackageManager().hasSystemFeature(
				PackageManager.FEATURE_CAMERA)) {
			return false;
		}
		return true;
	}

	private static Camera getCameraInstance() {
		Camera c = null;
		try {
			c = Camera.open();
		} catch (Exception e) {
			Log.e(HeartRateMonitor.HRM_TAG, "camera open fail");
		}
		return c;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.i(HRM_TAG, "onCreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		_imageView = (ImageView) findViewById(R.id.imageViewHeartRate);
	}

	@Override
	protected void onStart() {
		Log.i(HRM_TAG, "onStart");
		super.onStart();
	}

	@Override
	protected void onRestart() {
		Log.i(HRM_TAG, "onRestart");
		super.onRestart();
	}

	@Override
	protected void onResume() {
		Log.i(HRM_TAG, "onResume");
		super.onResume();
		if (_camera == null)
			_camera = getCameraInstance();

		if (_cameraPreview == null) {
			_cameraPreview = new HeartRateMonitorPreview(this, _camera,
					_imageView);
			_cameraPreview.start();
		}
	}

	@Override
	protected void onPause() {
		Log.i(HRM_TAG, "onPause");
		super.onPause();
		if (_cameraPreview != null) {
			_cameraPreview.stop();
			_cameraPreview = null;
		}
		if (_camera != null) {
			_camera.stopPreview();
			_camera.release();
			_camera = null;
		}
	}

	@Override
	protected void onStop() {
		Log.i(HRM_TAG, "onStop");
		super.onStop();
	}

	@Override
	protected void onDestroy() {
		Log.i(HRM_TAG, "onDestroy");
		super.onDestroy();
	}
}
