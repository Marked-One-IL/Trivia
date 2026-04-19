namespace Client
{
    public partial class GameWindow : System.Windows.Window
    {
        private System.Threading.Thread _thread = null;
        private System.Threading.CancellationTokenSource _cancellationTokenSource = null;
        private int _questionCount = 0;
        private int _currentQuestionCount = 1;
        private int _numOfCorrectAnswers = 0;
        private int _numOfIncorrectAnswers = 0;
        private bool _isGameDone = false;
        private static readonly int TIMEOUT = -1;
        private static readonly int GAME_FINISHED = -2;

        public GameWindow(int questionCount)
        {
            this.InitializeComponent();
            this._questionCount = questionCount;
            this.Dispatcher.Invoke(() => Refresh_Question());
        }

        private void Click_Answer0(object sender, System.Windows.RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
                j["answerId"] = 0;

                ClientSocket.SendMessage(Constants.Client.SUBMIT_ANSWER, j);
                j = ClientSocket.ReceiveMessage().Item2;

                if (int.Parse(j["correctIndex"].ToString()) == 0)
                {
                    this.increaseNumOfCorrectAnswers();
                }
                else
                {
                    this.increaseNumOfWrongAnswers();
                }

                Refresh_Question();
            });
        }
        private void Click_Answer1(object sender, System.Windows.RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
                j["answerId"] = 1;

                ClientSocket.SendMessage(Constants.Client.SUBMIT_ANSWER, j);
                j = ClientSocket.ReceiveMessage().Item2;

                if (int.Parse(j["correctIndex"].ToString()) == 1)
                {
                    this.increaseNumOfCorrectAnswers();
                }
                else
                {
                    this.increaseNumOfWrongAnswers();
                }

                Refresh_Question();
            });
        }
        private void Click_Answer2(object sender, System.Windows.RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
                j["answerId"] = 2;

                ClientSocket.SendMessage(Constants.Client.SUBMIT_ANSWER, j);
                j = ClientSocket.ReceiveMessage().Item2;

                if (int.Parse(j["correctIndex"].ToString()) == 2)
                {
                    this.increaseNumOfCorrectAnswers();
                }
                else
                {
                    this.increaseNumOfWrongAnswers();
                }

                Refresh_Question();
            });
        }
        private void Click_Answer3(object sender, System.Windows.RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
                j["answerId"] = 3;

                ClientSocket.SendMessage(Constants.Client.SUBMIT_ANSWER, j);
                j = ClientSocket.ReceiveMessage().Item2;

                if (int.Parse(j["correctIndex"].ToString()) == 3)
                {
                    this.increaseNumOfCorrectAnswers();
                }
                else
                {
                    this.increaseNumOfWrongAnswers();
                }

                Refresh_Question();
            });
        }

        private async void Refresh_Question()
        {
            if (this._currentQuestionCount > this._questionCount)
            {
                this.Close_Thread();

                await this.WaitingForResults();
                return;
            }
            ClientSocket.SendMessage(Constants.Client.GET_QUESTION);
            System.Text.Json.Nodes.JsonObject j = ClientSocket.ReceiveMessage().Item2;

            System.Text.Json.Nodes.JsonArray answers = j["answers"].AsArray();

            this.Dispatcher.Invoke(() => 
            {
                QuestionLabel.Content = j["question"].ToString();

                Answer0Button.Content = answers[0].ToString();
                Answer1Button.Content = answers[1].ToString();
                Answer2Button.Content = answers[2].ToString();
                Answer3Button.Content = answers[3].ToString();
            });

            this._currentQuestionCount++;

            this.Close_Thread();
            this._cancellationTokenSource = new System.Threading.CancellationTokenSource();
            this._thread = new System.Threading.Thread(new System.Threading.ParameterizedThreadStart(Refresh_Time));
            this._thread.Start(ClientHelper.timePerQuesion);
        }

        private void Refresh_Time(object obj)
        {
            int time = (int)obj;

            while (!this._cancellationTokenSource.Token.IsCancellationRequested && time > 0)
            {
                this.Dispatcher.Invoke(() =>
                {
                    TimeLabel.Content = time.ToString();
                });

                ClientHelper.CustomSleep(1000, this._cancellationTokenSource);
                time--;
            }

            // Time is up and user did not answer
            if (!this._cancellationTokenSource.Token.IsCancellationRequested)
            {
                this.Dispatcher.Invoke(() =>
                {
                    // Optionally mark no answer selected or highlight timeout visually
                    TimeLabel.Content = "0";

                    System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();

                    if (time <= 0)
                    {
                        j["answerId"] = GameWindow.TIMEOUT;
                        this.increaseNumOfWrongAnswers();
                    }
                    else
                    {
                        j["answerId"] = GameWindow.GAME_FINISHED;
                    }

                    ClientSocket.SendMessage(Constants.Client.SUBMIT_ANSWER, j);
                    ClientSocket.ReceiveMessage();

                    // Automatically proceed to next question
                    System.Threading.Thread thread = new System.Threading.Thread(new System.Threading.ThreadStart(Refresh_Question));
                    thread.Start();
                });
            }
        }

        private void Close_Thread()
        {
            if (this._cancellationTokenSource != null && this._thread != null)
            {
                this._cancellationTokenSource.Cancel();
                this._thread.Join();
            }
        }

        private void Click_GoBack(object sender, System.Windows.RoutedEventArgs e)
        {
            this.Close_Thread();

            if(!this._isGameDone)
            {
                ClientSocket.SendMessage(Constants.Client.LEAVE_GAME);
                ClientSocket.ReceiveMessage();
            }

            ClientHelper.SwitchToGrandparentWindow(this);
        }
        protected override void OnClosed(System.EventArgs e)
        {
            if (ClientHelper.doClose)
            {
                if(!this._isGameDone)
                {
                    ClientSocket.SendMessage(Constants.Client.LEAVE_GAME);
                    ClientSocket.ReceiveMessage();
                }

                ClientSocket.Destructor();
                base.OnClosed(e);
                System.Environment.Exit(0);
            }
            ClientHelper.doClose = true;
        }

        private void increaseNumOfCorrectAnswers()
        {
            this.Dispatcher.Invoke(() =>
            {
                this._numOfCorrectAnswers++;
                correctAnswers.Content = "Correct Answers - " + this._numOfCorrectAnswers;
            });
        }
        private void increaseNumOfWrongAnswers()
        {
            this.Dispatcher.Invoke(() =>
            {
                this._numOfIncorrectAnswers++;
                wrongAnswers.Content = "Wrong Answers - " + this._numOfIncorrectAnswers;
            });
        }
        private async Task WaitingForResults()
        {
            this.Dispatcher.Invoke(() =>
            {
                Answer0Button.Visibility = System.Windows.Visibility.Collapsed;
                Answer1Button.Visibility = System.Windows.Visibility.Collapsed;
                Answer2Button.Visibility = System.Windows.Visibility.Collapsed;
                Answer3Button.Visibility = System.Windows.Visibility.Collapsed;
                correctAnswers.Visibility = System.Windows.Visibility.Collapsed;
                wrongAnswers.Visibility = System.Windows.Visibility.Collapsed;
                TimeLabel.Visibility = System.Windows.Visibility.Collapsed;
                timeBorder.Visibility = System.Windows.Visibility.Collapsed;
                QuestionLabel.Visibility = System.Windows.Visibility.Collapsed;

                waitingLabel.Content = "Waiting For Others To Finish....";
                waitingLabel.Visibility = System.Windows.Visibility.Visible;
            });

            System.Text.Json.Nodes.JsonObject j;
            ClientSocket.SendMessage(Constants.Client.ARE_ALL_PLAYERS_DONE);
            j = ClientSocket.ReceiveMessage().Item2;

            while (int.Parse(j["areAllPlayersDone"].ToString()) == 0)
            {
                await Task.Delay(500);
                ClientSocket.SendMessage(Constants.Client.ARE_ALL_PLAYERS_DONE);
                j = ClientSocket.ReceiveMessage().Item2;
            }

            this.Dispatcher.Invoke(() =>
            {
                waitingLabel.Visibility = System.Windows.Visibility.Collapsed;
                ClientSocket.SendMessage(Constants.Client.GET_GAME_RESULTS);
                j = ClientSocket.ReceiveMessage().Item2;
                this._isGameDone = true;

                System.Text.Json.Nodes.JsonArray names = j["usernames"].AsArray();
                System.Text.Json.Nodes.JsonArray correctAnswersResult = j["correctAnswers"].AsArray();
                System.Text.Json.Nodes.JsonArray wrongAnswersResult = j["wrongAnswers"].AsArray();
                System.Text.Json.Nodes.JsonArray avgTime = j["avgTimePerQuestion"].AsArray();

                string content = "";
                string winner = "no one won";
                float maxScore = 0;
                for (int i = 0; i < names.Count; i++)
                {
                    content += "Name: " + names[i] + ", Correct Answers - " + correctAnswersResult[i].ToString() + ", Wrong Answers - " + wrongAnswersResult[i].ToString() + ", Average Time - " + avgTime[i].ToString();
                    float score = (int.Parse(correctAnswersResult[i].ToString()) * 100) / (float.Parse(avgTime[i].ToString()) * (int.Parse(correctAnswersResult[i].ToString()) + int.Parse(wrongAnswersResult[i].ToString())));
                    if(maxScore < score)
                    {
                        maxScore = score;
                        winner = names[i].ToString();
                    }
                    content += ", Final Score - " + score.ToString() + "\n";
                }
                ResultsLabel.Visibility = System.Windows.Visibility.Visible;
                ResultsLabel.Content = "RESULTS:\nThe Winner is - " + winner + "\n" + content;
            });
        }
    }
}
