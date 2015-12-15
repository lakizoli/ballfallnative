#pragma once

/// Base class of all quick time event.
class QTE {
private:
    bool mIsStarted;
    bool mIsPaused;

protected:
	QTE () : mIsStarted (false), mIsPaused (false) {}

public:
    virtual void Init () {}
    virtual void Shutdown () {}

    void Start () {
        if (mIsStarted)
            return;

        mIsStarted = true;
        mIsPaused = false;

        OnStart ();
    }

    void Stop () {
        if (!mIsStarted)
            return;

        mIsStarted = false;
        mIsPaused = false;

        OnStop ();
    }

    bool IsStarted () const {
        return mIsStarted;
    }

    void Pause () {
        if (!mIsStarted || mIsPaused)
            return;

        mIsPaused = true;

        OnPause ();
    }

    void Continue () {
        if (!mIsStarted || !mIsPaused)
            return;

        mIsPaused = false;

        OnContinue ();
    }

    bool IsPaused () const {
        return mIsPaused;
    }

    void Update (float elapsedTime) {
        if (!mIsStarted || mIsPaused)
            return;

        OnUpdate (elapsedTime);
    }

    void Render () {
        if (!mIsStarted)
            return;

        OnRender ();
    }

protected:
    virtual void OnStart () { }
    virtual void OnStop () { }
    virtual void OnPause () { }
    virtual void OnContinue () { }
    virtual void OnUpdate (float elapsedTime) { }
    virtual void OnRender () { }
};
