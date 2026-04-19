static class ClientHelper
{
    public static string current_user = "";
    public static bool doClose = true;
    public static int timePerQuesion = 0;

    public static void SwitchToNewWindow(System.Windows.Window parentWindow, System.Windows.Window childWindow)
    {
        childWindow.Owner = parentWindow;

        // Save x, y position.
        childWindow.Left = parentWindow.Left;
        childWindow.Top = parentWindow.Top;

        childWindow.Show();
        parentWindow.Hide();
    }

    // Each Child Window must have an method called SwitchInit() even if it does nothing.
    public static void SwitchToParentWindow(System.Windows.Window childWindow)
    {
        ClientHelper.doClose = false;

        // Save x, y position.
        childWindow.Owner.Left = childWindow.Left;
        childWindow.Owner.Top = childWindow.Top;

        childWindow.Owner.Show();
        ClientHelper.CallIfExists(childWindow.Owner, "SwitchInit");
        childWindow.Close(); // Will call OnClosed().
        // In each class's OnClosed() method ClientHelper.doClose will be used to check not to close the entire program.
        // .Hide() can be used as well but it will never will be freed.
    }

    public static void SwitchToGrandparentWindow(System.Windows.Window grandChildWindow)
    {
        ClientHelper.doClose = false;

        grandChildWindow.Owner.Left = grandChildWindow.Left;
        grandChildWindow.Owner.Top = grandChildWindow.Top;

        grandChildWindow.Owner.Show();
        ClientHelper.CallIfExists(grandChildWindow.Owner, "SwitchInit");

        System.Windows.Window fatherWindow = grandChildWindow.Owner;
        grandChildWindow.Close();

        ClientHelper.SwitchToParentWindow(fatherWindow);
    }

    // Sometimes when the flag is toggled the sleeping is still active.
    // So here it sleep in chunks and each chunk it checks for the flag.
    // So a delay will be less noticeable.
    public static void CustomSleep(uint milliseconds, System.Threading.CancellationTokenSource cancellationToken)
    {
        var stopwatch = System.Diagnostics.Stopwatch.StartNew();
        uint interval = 10; // smaller intervals improve accuracy

        while (stopwatch.ElapsedMilliseconds < milliseconds)
        {
            if (cancellationToken.Token.IsCancellationRequested)
                break;

            uint remaining = milliseconds - (uint)stopwatch.ElapsedMilliseconds;
            System.Threading.Thread.Sleep((int)Math.Min(interval, remaining));
        }
    }

    public static void Popup(object value)
    {
        Task.Run(() => System.Windows.MessageBox.Show($"{value}"));
    }

    // Restart the entire program in a brute force way.
    public static void ForceRestart()
    {
        // Recreate this application using the absolute path.
        System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo
        {
            FileName = Environment.ProcessPath,
            UseShellExecute = true
        };

        System.Diagnostics.Process.Start(startInfo); // Open new application.
        System.Environment.Exit(0); // Close current application.
    }

    // Try to find a method by name in a class.
    // If it exists call it.
    private static void CallIfExists(object myClass, string methodName)
    {
        var type = myClass.GetType();
        var method = type.GetMethod(methodName,
            System.Reflection.BindingFlags.Instance |
            System.Reflection.BindingFlags.Public |
            System.Reflection.BindingFlags.NonPublic);

        if (method != null)
        {
            method.Invoke(myClass, null);
        }
    }
}
