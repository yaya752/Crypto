#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>
#include <cmath>
using namespace std;

typedef array<pair<char, double>, 26> FreqArray;

class VigenereCryptanalysis
{
private:
  array<double, 26> targets;
  array<double, 26> sortedTargets;
  FreqArray freq;

  // TO COMPLETE

public:
  VigenereCryptanalysis(const array<double, 26> &targetFreqs)
  {
    targets = targetFreqs;
    sortedTargets = targets;
    sort(sortedTargets.begin(), sortedTargets.end());
  }

  // find the frequency(count) of each letter in the text
  void frequency(string input, FreqArray &freq)
  {
    for (int i = 0; i < 26; i++)
    {
      freq[i].first = 'a' + i;
      freq[i].second = 0;
    }

    for (char c : input)
    {
      freq[tolower(c) - 'a'].second++;
    }
  }
  int occur(string input, int i)
  {

    int occ = 0;
    for (char l : input)
    {
      if (l - 'a' == i)
      {
        occ++;
      }
    }
    return occ;
  }

  // IC calculation
  double Ic_determination(string input)
  {
    double ic = 0.0;
    frequency(input, freq);
    for (int j = 0; j < 26; j++)
    {
      int n = input.length();
      ic += ((freq[j].second) * (freq[j].second - 1)) / (n * (n - 1));
    }

    return ic;
  }

  // find the average of IC depending on key length
  double ic_avg(string input, int key_length)
  {
    double avg = 0.0;
    for (int j = 0; j < key_length; j++)
    {
      string text = "";
      for (int i = j; i < int(input.length()); i += key_length)
      {
        text += input[i];
      }
      avg = avg + Ic_determination(text);
    }
    return avg / key_length;
  }

  // find the key length based on the average of IC
  int keylength(string input)
  {
    pair<int, double> max = make_pair(2, ic_avg(input, 2));
    for (int key = 3; key < 15; key++)
    {
      double avgIC = ic_avg(input, key);
      if (avgIC > max.second)
      {
        max = make_pair(key, avgIC);
      }
      /*
      if (abs(avgIC-0.0778) < 0.01)
      {
        if (abs(max.second-0.0778) > 0.01)
        {
          max = make_pair(key, avgIC);
        }
      }*/
    }

    return max.first;
  }

  // Chi-squared calculation
  double Chisquared(string input)
  {
    double chiSquared = 0.0;
    for (int i = 0; i < 26; i++)
    {
      double Eq = double(targets[i] * input.length());
      double count = double(occur(input, i));
      chiSquared += pow(2.0, (count - Eq)) / Eq;
    }
    return chiSquared;
  }

  pair<string, string> analyze(string input)
  {
    for (int i = 0; i < int(input.length()); ++i)
    {
      if (input[i] >= 'A' && input[i] <= 'Z')
        input[i] = input[i] - ('Z' - 'z');
    }
    cout << input << endl;
    string key = "ISIMA PERHAPS";
    double tab_chis[26];
    string result = "";
    // string result = "I CAN NOT DECRYPT THIS TEXT FOR NOW :-)" + input;
    int key_length = keylength(input);
    string real_key = "";
    for (int j = 0; j < key_length; j++)
    {
      string text = "";
      for (int i = j; i < int(input.length()); i += key_length)
      {
        text += input[i];
      }
      for (int m = 0; m < 26; m += 1)
      {
        string test = text;
        for (int l = 0; l < int(test.length()); l++)
        {
          test[l] = (test[l] - ('a' + m) % 26) % 26 + 'a';
        }
        tab_chis[m] = Chisquared(test);
      }
      int min_letter = tab_chis[0];
      int index_min_letter = 0;
      for (int k = 0; k < 26; k++)
      {
        if (tab_chis[k] < min_letter)
        {
          min_letter = tab_chis[k];
          index_min_letter = k;
        }
      }
      real_key += 'a' + index_min_letter;
    }
    for (int i = 0; i < int(input.length()); ++i)
    {
      result += (input[i] - real_key[i % real_key.length()] + 26) % 26 + 'a';
    }
    return make_pair(result, real_key);
  }
};

int main()
{
  string input = "zbpuevpuqsdlzgllksousvpasfpddggaqwptdgptzweemqzrdjtddefekeferdprrcyndgluaowcnbptzzzrbvpssfpashpncotemhaeqrferdlrlwwertlussfikgoeuswotfdgqsyasrlnrzppdhtticfrciwurhcezrpmhtpuwiyenamrdbzyzwelzucamrptzqseqcfgdrfrhrpatsepzgfnaffisbpvblisrplzgnemswaqoxpdseehbeeksdptdttqsdddgxurwnidbdddplncsd";
  string input2 = "gmyxzoocxziancxktanmyolupjrztgxwshctzluibuicyzwxyqtvqxzukibkotuxkagbknmimmzzyajvjzampqyzloinoiqknaumbknknvkaiakgwtnilvvzvqydmvjcximrvzkilxzqtomrgqmdjrzyazvzmmyjgkoaknkuiaivknvvy";
  string input3 = "iefomntuohenwfwsjbsfftpgsnmhzsbbizaomosiuxycqaelrwsklqzekjvwsivijmhuvasmvwjewlzgubzlavclhgmuhwhakookakkgmrelgeefvwjelksedtyhsgghbamiyweeljcemxsohlnzujagkshakawwdxzcmvkhuwswlqwtmlshojbsguelgsumlijsmlbsixuhsdbysdaolfatxzofstszwryhwjenuhgukwzmshbagigzzgnzhzsbtzhalelosmlasjdttqzeswwwrklfguzl";
  string input4 = "MOMUDEKAPVTQEFMOEVHPAJMIICDCTIFGYAGJSPXYALUYMNSMYHVUXJELEPXJFXGCMJHKDZRYICUHYPUSPGIGMOIYHFWHTCQKMLRDITLXZLJFVQGHOLWCUHLOMDSOEKTALUVYLNZRFGBXPHVGALWQISFGRPHJOOFWGUBYILAPLALCAFAAMKLGCETDWVOELJIKGJBXPHVGALWQCSNWBUBYHCUHKOCEXJEYKBQKVYKIIEHGRLGHXEOLWAWFOJILOVVRHPKDWIHKNATUHNVRYAQDIVHXFHRZVQWMWVLGSHNNLVZSJLAKIFHXUFXJLXMTBLQVRXXHRFZXGVLRAJIEXPRVOSMNPKEPDTLPRWMJAZPKLQUZAALGZXGVLKLGJTUIITDSUREZXJERXZSHMPSTMTEOEPAPJHSMFNBYVQUZAALGAYDNMPAQOWTUHDBVTSMUEUIMVHQGVRWAEFSPEMPVEPKXZYWLKJAGWALTVYYOBYIXOKIHPDSEVLEVRVSGBJOGYWFHKBLGLXYAMVKISKIEHYIMAPXUOISKPVAGNMZHPWTTZPVXFCCDTUHJHWLAPFYULTBUXJLNSIJVVYOVDJSOLXGTGRVOSFRIICTMKOJFCQFKTINQBWVHGTENLHHOGCSPSFPVGJOKMSIFPRZPAASATPTZFTPPDPORRFTAXZPKALQAWMIUDBWNCTLEFKOZQDLXBUXJLASIMRPNMBFZCYLVWAPVFQRHZVZGZEFKBYIOOFXYEVOWGBBXVCBXBAWGLQKCMICRRXMACUOIKHQUAJEGLOIJHHXPVZWJEWBAFWAMLZZRXJEKAHVFASMULVVUTTGK";

  array<double, 26> french = {
      0.0811, 0.0081, 0.0338, 0.0428, 0.1769, 0.0113,
      0.0119, 0.0074, 0.0724, 0.0018, 0.0002, 0.0599,
      0.0229, 0.0768, 0.0520, 0.0292, 0.0083, 0.0643,
      0.0887, 0.0744, 0.0523, 0.0128, 0.0006, 0.0053,
      0.0026, 0.0012};
  
  array<double, 26> english = {
  0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228,
  0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025,
  0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987,
  0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
  0.01974, 0.00074};

VigenereCryptanalysis vc_en(english);

pair<string, string> output_en = vc_en.analyze(input4);

  VigenereCryptanalysis vc_fr(french);

  pair<string, string> output_fr = vc_fr.analyze(input);
  pair<string, string> output_fr2 = vc_fr.analyze(input2);
  pair<string, string> output_fr3 = vc_fr.analyze(input3);
  pair<string, string> output_fr4 = vc_fr.analyze(input4);

  cout << "Key number 1: " << output_fr.second << endl;
  cout << "Text: " << output_fr.first << endl;
  cout << "----------------" << endl;

  cout << "Key number 2: " << output_fr2.second << endl;
  cout << "Text: " << output_fr2.first << endl;
  cout << "--------------" << endl;

  cout << "Key number 3: " << output_fr3.second << endl;
  cout << "Text: " << output_fr3.first << endl;
  cout << "--------------" << endl;

  cout << "Key number 4: " << output_fr4.second << endl;
  cout << "Text: " << output_fr4.first << endl;

  cout << "--------------" << endl;

  cout << "Key number 4: " << output_en.second << endl;
  cout << "Text: " << output_en.first << endl;
}
