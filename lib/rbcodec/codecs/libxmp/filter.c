/*
 * Based on the public domain version by Olivier Lapicque
 * Rewritten for libxmp by Claudio Matsuoka
 *
 * Copyright (C) 2012 Claudio Matsuoka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LIBXMP_CORE_DISABLE_IT

#include "xmp.h"
#include "libxmp/common.h"
#include "mixer.h"

static const float filter_cutoff[] = {
	 130.8127899170F,  132.7154998779F,  134.6458892822F,  136.6043548584F,
	 138.5913085938F,  140.6071777344F,  142.6523437500F,  144.7272796631F,
	 146.8323822021F,  148.9681091309F,  151.1349029541F,  153.3332061768F,
	 155.5634918213F,  157.8262176514F,  160.1218566895F,  162.4508819580F,
	 164.8137817383F,  167.2110443115F,  169.6431884766F,  172.1107025146F,
	 174.6141204834F,  177.1539306641F,  179.7306976318F,  182.3449401855F,
	 184.9972076416F,  187.6880645752F,  190.4180450439F,  193.1877441406F,
	 195.9977111816F,  198.8485717773F,  201.7408905029F,  204.6752777100F,
	 207.6523437500F,  210.6727142334F,  213.7370300293F,  216.8459014893F,
	 220.0000000000F,  223.1999664307F,  226.4464874268F,  229.7402343750F,
	 233.0818786621F,  236.4721374512F,  239.9116973877F,  243.4012908936F,
	 246.9416503906F,  250.5334930420F,  254.1775970459F,  257.8746948242F,
	 261.6255798340F,  265.4309997559F,  269.2917785645F,  273.2087097168F,
	 277.1826171875F,  281.2143554688F,  285.3046875000F,  289.4545593262F,
	 293.6647644043F,  297.9362182617F,  302.2698059082F,  306.6664123535F,
	 311.1269836426F,  315.6524353027F,  320.2437133789F,  324.9017639160F,
	 329.6275634766F,  334.4220886230F,  339.2863769531F,  344.2214050293F,
	 349.2282409668F,  354.3078613281F,  359.4613952637F,  364.6898803711F,
	 369.9944152832F,  375.3761291504F,  380.8360900879F,  386.3754882812F,
	 391.9954223633F,  397.6971435547F,  403.4817810059F,  409.3505554199F,
	 415.3046875000F,  421.3454284668F,  427.4740600586F,  433.6918029785F,
	 440.0000000000F,  446.3999328613F,  452.8929748535F,  459.4804687500F,
	 466.1637573242F,  472.9442749023F,  479.8233947754F,  486.8025817871F,
	 493.8833007812F,  501.0669860840F,  508.3551940918F,  515.7493896484F,
	 523.2511596680F,  530.8619995117F,  538.5835571289F,  546.4174194336F,
	 554.3652343750F,  562.4287109375F,  570.6093750000F,  578.9091186523F,
	 587.3295288086F,  595.8724365234F,  604.5396118164F,  613.3328247070F,
	 622.2539672852F,  631.3048706055F,  640.4874267578F,  649.8035278320F,
	 659.2551269531F,  668.8441772461F,  678.5727539062F,  688.4428100586F,
	 698.4564819336F,  708.6157226562F,  718.9227905273F,  729.3797607422F,
	 739.9888305664F,  750.7522583008F,  761.6721801758F,  772.7509765625F,
	 783.9908447266F,  795.3942871094F,  806.9635620117F,  818.7011108398F,
	 830.6093750000F,  842.6908569336F,  854.9481201172F,  867.3836059570F,
	 880.0000000000F,  892.7998657227F,  905.7859497070F,  918.9609375000F,
	 932.3275146484F,  945.8885498047F,  959.6467895508F,  973.6051635742F,
	 987.7666015625F, 1002.1339721680F, 1016.7103881836F, 1031.4987792969F,
	1046.5023193359F, 1061.7239990234F, 1077.1671142578F, 1092.8348388672F,
	1108.7304687500F, 1124.8574218750F, 1141.2187500000F, 1157.8182373047F,
	1174.6590576172F, 1191.7448730469F, 1209.0792236328F, 1226.6656494141F,
	1244.5079345703F, 1262.6097412109F, 1280.9748535156F, 1299.6070556641F,
	1318.5102539062F, 1337.6883544922F, 1357.1455078125F, 1376.8856201172F,
	1396.9129638672F, 1417.2314453125F, 1437.8455810547F, 1458.7595214844F,
	1479.9776611328F, 1501.5045166016F, 1523.3443603516F, 1545.5019531250F,
	1567.9816894531F, 1590.7885742188F, 1613.9271240234F, 1637.4022216797F,
	1661.2187500000F, 1685.3817138672F, 1709.8962402344F, 1734.7672119141F,
	1760.0000000000F, 1785.5997314453F, 1811.5718994141F, 1837.9218750000F,
	1864.6550292969F, 1891.7770996094F, 1919.2935791016F, 1947.2103271484F,
	1975.5332031250F, 2004.2679443359F, 2033.4207763672F, 2062.9975585938F,
	2093.0046386719F, 2123.4479980469F, 2154.3342285156F, 2185.6696777344F,
	2217.4609375000F, 2249.7148437500F, 2282.4375000000F, 2315.6364746094F,
	2349.3181152344F, 2383.4897460938F, 2418.1584472656F, 2453.3312988281F,
	2489.0158691406F, 2525.2194824219F, 2561.9497070312F, 2599.2141113281F,
	2637.0205078125F, 2675.3767089844F, 2714.2910156250F, 2753.7712402344F,
	2793.8259277344F, 2834.4628906250F, 2875.6911621094F, 2917.5190429688F,
	2959.9553222656F, 3003.0090332031F, 3046.6887207031F, 3091.0039062500F,
	3135.9633789062F, 3181.5771484375F, 3227.8542480469F, 3274.8044433594F,
	3322.4375000000F, 3370.7634277344F, 3419.7924804688F, 3469.5344238281F,
	3520.0000000000F, 3571.1994628906F, 3623.1437988281F, 3675.8437500000F,
	3729.3100585938F, 3783.5541992188F, 3838.5871582031F, 3894.4206542969F,
	3951.0664062500F, 4008.5358886719F, 4066.8415527344F, 4125.9951171875F,
	4186.0092773438F, 4246.8959960938F, 4308.6684570312F, 4371.3393554688F,
	4434.9218750000F, 4499.4296875000F, 4564.8750000000F, 4631.2729492188F,
	4698.6362304688F, 4766.9794921875F, 4836.3168945312F, 4906.6625976562F,
	4978.0317382812F, 5050.4389648438F, 5123.8994140625F, 5198.4282226562F
};

static const float dmpfac[] = {
	1.0000000000F, 0.9812888503F, 0.9629278779F, 0.9449104071F,
	0.9272300601F, 0.9098805189F, 0.8928556442F, 0.8761492968F,
	0.8597555757F, 0.8436685801F, 0.8278825879F, 0.8123919964F,
	0.7971912026F, 0.7822748423F, 0.7676376104F, 0.7532742620F,
	0.7391796708F, 0.7253487706F, 0.7117766738F, 0.6984585524F,
	0.6853895783F, 0.6725651622F, 0.6599807143F, 0.6476317644F,
	0.6355138421F, 0.6236226559F, 0.6119539738F, 0.6005036235F,
	0.5892674923F, 0.5782416463F, 0.5674220920F, 0.5568050146F,
	0.5463865399F, 0.5361630321F, 0.5261308551F, 0.5162863135F,
	0.5066260099F, 0.4971464872F, 0.4878443182F, 0.4787161946F,
	0.4697588682F, 0.4609691501F, 0.4523439109F, 0.4438800514F,
	0.4355745614F, 0.4274244606F, 0.4194268584F, 0.4115789235F,
	0.4038778245F, 0.3963208199F, 0.3889051974F, 0.3816283345F,
	0.3744876385F, 0.3674805760F, 0.3606045842F, 0.3538572788F,
	0.3472362161F, 0.3407390118F, 0.3343634009F, 0.3281070888F,
	0.3219678402F, 0.3159434497F, 0.3100318015F, 0.3042307496F,
	0.2985382676F, 0.2929522693F, 0.2874708176F, 0.2820919156F,
	0.2768136561F, 0.2716341615F, 0.2665515840F, 0.2615641057F,
	0.2566699386F, 0.2518673539F, 0.2471546233F, 0.2425300926F,
	0.2379920781F, 0.2335389853F, 0.2291692048F, 0.2248811871F,
	0.2206734121F, 0.2165443599F, 0.2124925703F, 0.2085165977F,
	0.2046150118F, 0.2007864416F, 0.1970295012F, 0.1933428496F,
	0.1897251904F, 0.1861752123F, 0.1826916784F, 0.1792733073F,
	0.1759189069F, 0.1726272553F, 0.1693972051F, 0.1662275940F,
	0.1631172895F, 0.1600651890F, 0.1570701897F, 0.1541312188F,
	0.1512472481F, 0.1484172493F, 0.1456401944F, 0.1429150999F,
	0.1402409971F, 0.1376169324F, 0.1350419670F, 0.1325151771F,
	0.1300356686F, 0.1276025623F, 0.1252149642F, 0.1228720546F,
	0.1205729842F, 0.1183169261F, 0.1161030829F, 0.1139306650F,
	0.1117988899F, 0.1097070053F, 0.1076542661F, 0.1056399345F,
	0.1036632955F, 0.1017236337F, 0.0998202711F, 0.0979525223F,
	0.0961197242F, 0.0943212137F, 0.0925563574F, 0.0908245221F
};


/*
 * Simple 2-poles resonant filter
 */
void filter_setup(int srate, int cutoff, int res, int *a0, int *b0, int *b1)
{
	float fc, fs = (float)srate;
	float fg, fb0, fb1;
	float d2, d, e;

	/* [0-255] => [100Hz-8000Hz] */
	CLAMP(cutoff, 0, 255);

	CLAMP(res, 0, 255);

	fc = filter_cutoff[cutoff];

	fc *= 3.14159265358979 * 2 / fs;
	d2 = dmpfac[res >> 1];
	d = (1.0 - d2) * fc;

	if (d > 2.0)
		d = 2.0;

	e = 1.0 / (fc * fc);
	d = (d2 - d) / fc + e;

	fg  = 1.0 / (1 + d);
	fb0 = (d + e) / (1 + d);
	fb1 = -e / (1 + d);

	*a0 = (int)(fg  * (1 << FILTER_SHIFT));
	*b0 = (int)(fb0 * (1 << FILTER_SHIFT));
	*b1 = (int)(fb1 * (1 << FILTER_SHIFT));
}

#endif
