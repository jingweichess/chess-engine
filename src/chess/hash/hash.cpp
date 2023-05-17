/*
    Jing Wei, the rebirth of the chess engine I started in 2010
    Copyright(C) 2019-2023 Chris Florin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdio>

#include "hash.h"

#include "../../game/types/color.h"
#include "../../game/types/hash.h"

#include "../types/castlerights.h"
#include "../types/piecetype.h"
#include "../types/square.h"

std::array<std::array<std::array<Hash, Square::SQUARE_COUNT>, PieceType::PIECETYPE_COUNT>, Color::COLOR_COUNT> PieceHashList {
    {
        {
            {
                {},
                {
                    0xdf809952ab2dfbd8, 0xcc16df729324ee47, 0x3b53c358ea6d9cca, 0xe2a00246c70646d1, 0xd94959165448a30c, 0xe718f006470b1e6b, 0x2106c52770b9ef1e, 0xb48623d51a2c6b95,
                    0xacabd8ec744e5580, 0x4bc1e53c091708cf, 0x536888f7924dbeb2, 0x289dc9bf16465499, 0xf51449146ec94734, 0x8b52c77a2c5fd173, 0xefacdb2659165f86, 0x2b45085e1ab7c5dd,
                    0x2ad42ad79a7eec28, 0x67fa360d6a26dc57, 0x3f829cd6fa2e659a, 0x6de7baa2d098c361, 0xc0cc9e90eea3f85c, 0x3c812cb31ffbcd7b, 0xe8aeed079e31a4ee, 0x19fc93020d799125,
                    0xfff60de41b505fd0, 0x2a4cefee282088df, 0x0ed56d0906d13182, 0xa4ffcec55a66b329, 0x7b0e693441335684, 0xbec41bcb702d3283, 0x4887bd3c65a75f56, 0x5c974a60e32ced6d,
                    0xcf07cf05bc875078, 0x5cdb23c5b1f42e67, 0xbaec3d87f84bc26a, 0x1e14f704d1dd43f1, 0x1843c6c5b74601ac, 0xf35a31a5f2a6208b, 0xd55449d43da72ebe, 0xa85a93f85a90fab5,
                    0x1b7f09b5769c5e20, 0x2052b6d0ac35ecef, 0xf0c2d358d887b852, 0x5d9795a8fb6d95b9, 0x9d6923ae979e99d4, 0xdf68cda5c4fcb793, 0x5c54ecc97374b326, 0x553b98f8b4e8d8fd,
                    0xdaa57560af3c28c8, 0x695bb9b3b99de477, 0xdb24c6d039c2b33a, 0xbfdbef7a3e4cc881, 0x818c79c80173befc, 0x6d1513bc9d2b179b, 0xa412acf4b9e78c8e, 0x295549327a7ba845,
                    0x9804bc3043c75070, 0xd891042dd38834ff, 0x8e31de53450e5322, 0xb78f5662b0f3fc49, 0x9a7dfa5d9ef01124, 0xcbf909f1aa0f60a3, 0x2b4202a21feb5af6, 0x5eac480f4f14888d,
                },
                {
                    0xd1d042b163d27518, 0x31ca3eac3274fe87, 0x14be702f18d0380a, 0x771b34ab35a05111, 0xb753ef8f27b2304c, 0x18208ac303ebb2ab, 0xc614440156ffbe5e, 0x8611b7c7d68299d5,
                    0xce469b2a3ba636c0, 0x71d96541c688610f, 0x26eb4f60774201f2, 0x2c574c26a6d2e6d9, 0xf3ed287ab34cbc74, 0xc13f4c03ace62db3, 0x8dfb46eb49b856c6, 0xfef64748a918fc1d,
                    0x6907daa46ebf3568, 0xdedf39c4400a7c97, 0x75863465c4f150da, 0x2ac370dd13d0dda1, 0xe6d1f2d9dbc6559c, 0x3644c8182188f1bb, 0x8d0486b7023cc42e, 0x0c0c9f78582ecf65,
                    0x756f28f7624e1110, 0xbd1a0d10a7e7711f, 0x58eee875573fc4c2, 0x6ca9c3090d235569, 0xb4a07bfbb0199bc4, 0x597d6db379421ec3, 0x43974617e0c8a696, 0xeb864cd0829f33ad,
                    0x3bd9b05c57b769b8, 0x96c9c1b9882f5ea7, 0xf363b6b212e2fdaa, 0xe5e5f07901176e31, 0xae23724576b52eec, 0xeedb187716e3d4cb, 0xa2e0b63f222b9dfe, 0xe2718a7bf94948f5,
                    0x96c884565713df60, 0xa3cbeca0a496652f, 0xe65dcc385a649b92, 0xe39d51a6a83e47f9, 0x6f75e7a43ffbaf14, 0x26b27b16e92433d3, 0x488725bab6494a66, 0xc359e46bf3902f3d,
                    0x29f332f7e9b01208, 0xb764823e42f4a4b7, 0x6f513d1d4ca23e7a, 0x0817cb1681ed02c1, 0x24fac49948c3bc3c, 0x7340d1e43b1d5bdb, 0x21ee9acb66994bce, 0xd5fa7e1653db0685,
                    0x414a76d0a48ca1b0, 0x36db919803c63d3f, 0x299959ba534d8662, 0xbf6337d160bcbe89, 0xdedd92cb2cd7f664, 0xfccdeae114cd6ce3, 0x22a3062c4ca74236, 0x30dfebcefc14eecd,
                },
                {
                    0xbdd2eb0c82de2e58, 0x48e828294aab4ec7, 0x6f1ddcdf016c134a, 0xfbdbc60a070a9b51, 0xf45b0545e976fd8c, 0xcf834ebe2b9686eb, 0x96336c859892cd9e, 0xedb9f4ee322d0815,
                    0xe803b68f108d5800, 0xe7249c3f36e7f94f, 0x5e7540c341d78532, 0xd59c1b40d377b919, 0x9ede6bd3afd371b4, 0x3d831c6562bec9f3, 0x3162fe04a78f8e06, 0x606445361496725d,
                    0x4571de1464b6bea8, 0x0afa258a2be45cd7, 0xa0b935d3d5bd7c1a, 0xced949777d6937e1, 0xd2632e188793f2dc, 0xaff0f6f2dbf055fb, 0x982614d36a65236e, 0xb9090c291ec84da5,
                    0xfd2350a1ec2b0250, 0xd5a9dde6e9ac995f, 0x6150cbb3231f9802, 0x14099b46e18837a9, 0xf76b825ac1532104, 0xc1263c3e97b94b03, 0xb01e7c691bef2dd6, 0xb802c943bdbdb9ed,
                    0x8aa9ccd103eec2f8, 0x2690fea13570cee7, 0xa2a55a08535378ea, 0x794b00e8de41d871, 0x6cccd322391f9c2c, 0x91f99858a60bc90b, 0x5c8934ee479d4d3e, 0x3a3c0c4af475d735,
                    0x1c7a70bd63baa0a0, 0xe6d670b212051d6f, 0x01b09bf6c182bed2, 0x37cf748840473a39, 0x48bbae1868fc0454, 0x72c1f099e6bdf013, 0x46c51f2e1ff321a6, 0x35cdc1221073c57d,
                    0xd2d821a1587b3b48, 0x59d190bb8861a4f7, 0xe2d8be57b92b09ba, 0xe4440068bf0d7d01, 0x9bb1a5d34b5ef97c, 0x27968f2b5a09e01b, 0x5b5a9ecf25084b0e, 0xfeb7b6d96e3ea4c5,
                    0xe7b1fb69ced132f0, 0x354659c40022857f, 0x99875610849df9a2, 0xc1732db9094dc0c9, 0x9a6c47750db31ba4, 0x856b6e26010db923, 0x17ed49cd1b086976, 0x8c1471e84de1950d,
                },
                {
                    0x6f48a3cc2d912798, 0xc3c303f32807df07, 0x3bd7511ecb812e8a, 0xd52e8720e1852591, 0x526a244a92d70acc, 0xcc517fad4e4b9b2b, 0xe3972d6e50b31cde, 0x9f8a01c3b76bb655,
                    0x8822afa80043b940, 0x89d786d26e75d18f, 0x58c0fa35414e4872, 0x0e55865d4a74cb59, 0x449ef6d2c59d66f4, 0x71c0d6a62629a633, 0x854e0a2e35dc0546, 0xc68682446f70289d,
                    0x264e44be71a587e8, 0xb08158d309f47d17, 0xe3171402a3d2e75a, 0xd43bb87cc3a1d221, 0xe1e719d8bb4cd01c, 0x6deea1ec6f71fa3b, 0xe733295741eac2ae, 0xd1c1298dfb860be5,
                    0x9241c50996273390, 0x8416381b91b0019f, 0xb1d9e67d09b0ab42, 0xad7d208d95d559e9, 0xdbf9df8aa61fe644, 0xd074022933d2b743, 0xb9c318462a5af516, 0xb9514368b6c8802d,
                    0x8b9b9c3d866d5c38, 0x4f70c2df25f87f27, 0xcfe1d5cb80dd342a, 0xef5fe4d02f9c82b1, 0x72c6d5b797c5496c, 0xdcbde0ab505dfd4b, 0x5618a690693c3c7e, 0x9f644340f656a575,
                    0xb8e73b3d49d0a1e0, 0x037a27c028c215af, 0xa44fbfa7fd222212, 0xf1361ebf91c86c79, 0xaa78ab9e13df9994, 0xcdca13e4b609ec53, 0xbdd2fd8813b238e6, 0x7944263d3dd39bbd,
                    0x7167e88d90dda488, 0x77de41008624e537, 0x92d2e953969d14fa, 0xe7eb3a81cbee3741, 0xd9963cf5728576bc, 0xbca44c6e3a30a45b, 0xd43e88d700748a4e, 0x9d28b71d83e68305,
                    0x4f56500f3fd50430, 0x0b5bff818cdd0dbf, 0x588fbd77183face2, 0xed46ae9288e70309, 0x43543d1c12c180e4, 0x7e0e7bb3f7104563, 0xc558b12b3e4ed0b6, 0x6589e5d586ba7b4d,
                },
                {
                    0x6738a78bc92b60d8, 0xf17424d556caaf47, 0x37ccf689de4f89ca, 0xe9d2f8b9ab4fefd1, 0xc12d1c945d12580c, 0xec0da3ac3c4aef6b, 0x49fc1b2edaa0ac1e, 0x39c84d14307ea495,
                    0xfbe3d0dd58095a80, 0xad0cede3c171e9cf, 0xb2ab179904e64bb2, 0x0644b99ffa0a1d99, 0x5bee0a3a95ea9c34, 0xf01e453b0f66c273, 0xd66b3644f7ddbc86, 0xa494302a0be61edd,
                    0x32c31dbccacb9128, 0x80f379e4f67add57, 0xd2d14288e671929a, 0x65537c93dcbaac61, 0x51e460dd8030ed5c, 0xcd23f1243c4dde7b, 0x929e67c6340da1ee, 0xde33b400c8a80a25,
                    0x26593c7f7d82a4d0, 0x610c6bb38431a9df, 0x418f902aea32fe82, 0x75f53550284abc29, 0x3aa36923cfbfeb84, 0x9d66d5adf5ce6383, 0x36e708b65f4bfc56, 0x2277d615cfff866d,
                    0xc744604ee4733578, 0xa2bd7db806066f67, 0x577f0bc1a2c02f6a, 0x09b7c1d6fb676cf1, 0xac8416e86be636ac, 0x13e98156061a718b, 0x37f5bd2d82486bbe, 0xc5c5aca9e92bb3b5,
                    0x90c6eaa3f695e320, 0x85fa82f05d0d4def, 0x53c758cd3c82c552, 0x3689cab2ab01deb9, 0xf2b4507781e66ed4, 0xe9f9793b8f482893, 0x8b1665ee34c69026, 0xe327cb97edefb1fd,
                    0xebe9b71068174dc8, 0xc7e299d4787e6577, 0xe72ed73a3c38603a, 0x36c7302ebecf3181, 0x79365d56677733fc, 0x351fb01f5bd1a89b, 0x8e130de3441e098e, 0x7f1341848f12a145,
                    0x1b5aad9fb4d81570, 0x5d97281aae35d5ff, 0x14d8cf4c8d72a022, 0x270dcf5afdc88549, 0x7e0051804d432624, 0xefab611cbf1511a3, 0x90debe7ea9ba77f6, 0x9abd815928dfa18d,
                },
                {
                    0xf66e6b59faecda18, 0x7ba6909da333bf87, 0x6e9acbe0e117250a, 0x859ecdea8aaafa11, 0x218ba941c168e54c, 0xf2434f7d05d483ab, 0x74be1733d19b7b5e, 0x2cfdb43ba7a5d2d5,
                    0xe803a3b2a51e3bc0, 0xb9e176e5c41c420f, 0x7b43cede5bdf8ef2, 0x725fc8411077afd9, 0xc643d11d01fb1174, 0xf09ff34776b61eb3, 0x3b92dd8730d4b3c6, 0xbd4d60757c38551d,
                    0x2fef58ece568da68, 0x72b3ce184db77d97, 0xcdd278f294d97dda, 0x4d8f313ffef3c6a1, 0xc4ac04611f804a9c, 0x6a55a86fe2c402bb, 0x584e7c6d2c0dc12e, 0x284309fba06e4865,
                    0xdd2f2cbfff7d5610, 0x823be34de571921f, 0x92cf92f1e3e691c2, 0xd3b0efa5d7285e69, 0x0bb6c45def7330c4, 0xac118cc5c5ec4fc3, 0xf91292f24e024396, 0x5aa67189aba2ccad,
                    0x1b1fd5c563404eb8, 0x8e920d92c1da9fa7, 0x6fd5a476003c6aaa, 0x49fcc91187e29731, 0x4230295ecec263ec, 0x31a75705f78125cb, 0x95834366ce01dafe, 0xcff25f81f73501f5,
                    0xdbbe397a974a6460, 0x2e5cb0126326c62f, 0x65616894eee4a892, 0xc3b076fbda3390f9, 0x027575d634508414, 0x8fb317b0eab8a4d3, 0x6d39ed8766702766, 0xe4cc7804d308083d,
                    0x5886c5e0f3683708, 0xbb5ec530dbae25b7, 0xbe9efeca413ceb7a, 0xc7345638edf06bc1, 0x8ecca3641374313c, 0xe06882877f2cecdb, 0xf7b55f5d7b44c8ce, 0x6ee16df0ca02ff85,
                    0xcf7569052b1a66b0, 0x2ec43b93a86cde3f, 0x81e9ab6ca376d362, 0x6c447dd3c6324789, 0xaef318a10e780b64, 0x849b15d0615c1de3, 0x48cb51d0908b5f36, 0x35fc0abdf69107cd,
                },
                {
                    0xc335eff8a8159358, 0x9f40085c19830fc7, 0xd8a4d849bb18004a, 0x01a9ae55e5d64451, 0x99f855d8791ab28c, 0xec6032c7fb2857eb, 0x7a53372310e38a9e, 0x6188e96667214115,
                    0x069daa06b4c25d00, 0xdc74f4154ab4da4f, 0xf919cb42557a1232, 0xf9fef6ccfbfd8219, 0xafdd2e1b2b0ec6b4, 0x044f00dcf457baf3, 0x60f8fad46400eb06, 0x909872cd92a6cb5d,
                    0x0a88e82f76bd63a8, 0xbe94d217abea5dd7, 0xe9e33600e64aa91a, 0xb12b2120a08d20e1, 0x11071155227ae7dc, 0x21dac39b431466fb, 0x99f453a2552b206e, 0x5e85a258dd18c6a5,
                    0xe94e5432b9574750, 0xd7bb046419afba5f, 0xae8b5724560b6502, 0x0658f38a20ae40a9, 0x754e034ff679b604, 0x5d3c2167cc6c7c03, 0x287bb1a3c9bdcad6, 0xac8359ab2bf252ed,
                    0xf7e425b48814a7f8, 0x617fe83885b50fe7, 0x205fe2792091e5ea, 0x143819405b4e0171, 0xf6d01acc1999d12c, 0x4e9db76e94d21a0b, 0x2a85a3b6c7a88a3e, 0x306492b58ab29035,
                    0x2eecef45992e25a0, 0x9bf409e02f4e7e6f, 0x26c54b1ac387cbd2, 0xe82b10a9ad9d8339, 0x15f3cb1aec5dd954, 0xb9b13d65809b6113, 0x76bdc8bbcbeefea6, 0xf60e8f8edf5c9e7d,
                    0x2e28185988106048, 0x9314c8816bf425f7, 0xf6c657977ceab6ba, 0x3addd1a5ef91e601, 0x0ae7c9e39fbc6e7c, 0x537d6a05a482711b, 0xab1002587128c80e, 0x95583fc4aef79dc5,
                    0x04d21c74dfdbf7f0, 0x607964eaffc2267f, 0x9b221d70598c46a2, 0x3cd5fac2806449c9, 0x36e919fce7a030a4, 0xbd42bf5d26256a23, 0x790aa83b66018676, 0xaef87316f20eae0d,
                }
            },
        },
        {
            {
                {},
                {
                    0xf797531cf5e58c98, 0x642850a305f8a007, 0x13369e8f93921b8a, 0xd041386a6311ce91, 0x0cb2a3857d67bfcc, 0x7a0f645bac866c2b, 0x406bec1bb3b8d9de, 0x9cfe19d0f930ef55,
                    0x2fb657539435be40, 0x0911f4b9697bb28f, 0x27fe488f40f5d572, 0x781a45646adb9459, 0x0662c8a57265bbf4, 0x8669373b608b9733, 0x10ec77ed54a26246, 0x6db3c3316171819d,
                    0x1a4a20a874092ce8, 0x9791121fed537e17, 0xd0af82ea5205145a, 0x3a51583177c6bb21, 0xb92795a25260c51c, 0x3fe252a87d7f0b3b, 0xb2d691051aa5bfae, 0x0172c29318e784e5,
                    0xfe8c832a88507890, 0xcea24f89c52c229f, 0x73cc4e71dfe17842, 0x3ebb0d1cc31c62e9, 0xd00e823016137b44, 0xb92055c9718ee843, 0x15eab425e5be9216, 0x469be449732e192d,
                    0x1ef516c318304138, 0x677d8314bdd5c027, 0x4a99b5a0cb00a12a, 0x9c01e1103be9abb1, 0xa9f63928e5ac7e6c, 0x6ba2dd898e4d4e4b, 0xd2cdafb12a7c797e, 0xe39c63614de45e75,
                    0xdcc379c4a98126e0, 0x4f34c83df5c476af, 0x4ab072a7a9ac2f12, 0x44b31d7ef37fb579, 0x68048315ab4e6e94, 0x0f52c3c849305d53, 0x43b5ba74c88315e6, 0x81a6e1b9452d74bd,
                    0x6dc37db7bb4fc988, 0x8f707ce425906637, 0xffe6bd4b0681c1fa, 0x1be1a9f799f3a041, 0xadf53f31758febbc, 0xd6a2e74f0c12355b, 0x5bd8e6d0310a074e, 0x46ca7c22f8307c05,
                    0x678b10af505cc930, 0xbee4d2597875aebf, 0x6e1ba7edeef2f9e2, 0xdf0094310a9e8c09, 0xc2864dd1a9fb95e4, 0xe8f575af95b0f663, 0x64749b2add5cedb6, 0xe1a6b47f5d98944d,
                },
                {
                    0x9551a0af499cc5d8, 0x986eb9c6f4d47047, 0x71f3d062d1c576ca, 0xcae543a2e89d98d1, 0x813c6e5d07900d0c, 0x8256ee6cea2ec06b, 0x9bf869f6155b691e, 0xe992d4082814dd95,
                    0x11ac49ee90b85f80, 0xe5bb956374b0cacf, 0x56826e5cad92d8b2, 0xf6d739fc4b51e699, 0xbf6b5a3b793ff134, 0x9bdb5d10d391b373, 0xd563bcb405f91986, 0xf43b98383ad877dd,
                    0x6c6e5bff128c3628, 0xf0f242402e32de57, 0x16edf69a8f48bf9a, 0xa9d1f60a7ae09561, 0xab1cd568b871e25c, 0xf98b960ff243ef7b, 0xa85246be27bd9eee, 0xed2b75052e1a8325,
                    0x8fc7a52589a8e9d0, 0x551dc0accc26cadf, 0xf2c19f2760a8cb82, 0x14660ae1bcb2c529, 0xac140493bf808084, 0xf680989e5d939483, 0x2e3c1dc4f5449956, 0x6739775be3961f6d,
                    0xfc367e6b18d31a78, 0x0452fb75167cb067, 0x23d20e4806c89c6a, 0x9ad9c15a2ff595f1, 0x6d4917f50c3a6bac, 0x445395d6d432c28b, 0x8f68a742f1bda8be, 0x6569db122b0a6cb5,
                    0x6f5fc632b5836820, 0xae0cf07a2ac8aeef, 0xa10862f1d091d252, 0x4e48a632ba1a27b9, 0x7a024145b26243d4, 0xad5283d67cb79993, 0xedf4435cff6c6d26, 0xbd72b7bf76ba8afd,
                    0xb157d25c626672c8, 0xb17dc76944c2e677, 0xab5092e335420d3a, 0x8cad3b6c03559a81, 0xcdeefa813e2ea8fc, 0x7ffc91af361c399b, 0x9aa4beea0628868e, 0xae07c02e9fed9a45,
                    0x51f9e64039dcda70, 0xd431359216c776ff, 0xc433cfb8e2eaed22, 0x94399fad83210e49, 0x56ceda5c66ca3b24, 0x63504752783ec2a3, 0xed551f9be9dd94f6, 0xb59af05abb6eba8d,
                },
                {
                    0xacd4e40b487b3f18, 0x930de81eb2568087, 0xe8da41991cf2120a, 0xe08c62c69cb9a311, 0x9242929a90d39a4c, 0x1db79ad6c46154ab, 0x20534e83d10b385e, 0x81d62e28fe0d0bd5,
                    0x3ed0c448378a40c0, 0x6461902f0094230f, 0x8a6deb4f6a911bf2, 0x8ba6ea9dcba078d9, 0x68193bac20dd6674, 0xd145cab9a5aa0fb3, 0x4d967e6bbb4510c6, 0x0e445151b11bae1d,
                    0xa27dd99dc7867f68, 0xe7269699cac87e97, 0xdde7f8f29555aada, 0xf9b3c01fe01aafa1, 0x2c0ac03f9dee3f9c, 0x87105b0041a313bb, 0x7850ecbf67b2be2e, 0x921fbf2a36f1c165,
                    0x4d1a0a0d1aa09b10, 0x9c346f5552dfb31f, 0xca970f6ef7a15ec2, 0x8f65d8014bb16769, 0x1f4712afa400c5c4, 0x5e59295878ba80c3, 0xfabfc4fe8b8fe096, 0xcddac7421f6a65ad,
                    0xf97df239cf3d33b8, 0xefc4fec97be9e0a7, 0x3f2d708f1b29d7aa, 0x0de76f637db1c031, 0xdf1dd577a68398ec, 0x12cc2a9c96c276cb, 0x2c957ff258ac17fe, 0x03a336054c64baf5,
                    0x82b15a85ea74e960, 0xf703858d829b272f, 0x6185ec5ca778b592, 0xdcf060b04facd9f9, 0x1970471982d95914, 0x6d290a5b937115d3, 0xfae3112053eb0466, 0x98f022d52643e13d,
                    0x4dd8810b92945c08, 0xc0ad115345cba6b7, 0xa3d3a5f3a06b987a, 0xb8d1e93d81f7d4c1, 0x6f6c911de2d8a63c, 0x73589347e2e07ddb, 0x4f8f96347bc445ce, 0x0747d74ae06ef885,
                    0x1b1f7ebe999c2bb0, 0xffc084021ef77f3f, 0x427fa721f4b42062, 0x1551b48a482bd089, 0x03ab11196f4c2064, 0xd6507daed60ecee3, 0xf872055cbec37c36, 0xb02ccd94cdd120cd,
                },
                {
                    0x747b793fd7c0f858, 0x028ddc434abed0c7, 0x8e151b6d5c57ed4a, 0x8270a628e5a5ed51, 0x3138d1e0d272678c, 0xbd8eff5a8b5e28eb, 0xd0d188cfc208479e, 0x02f52c20c5597a15,
                    0x7567662c55eb6200, 0x19fa8d01e165bb4f, 0x936ed05987309f32, 0x7a3447a65a074b19, 0xb71332558a7e1bb4, 0xa7cb3e806f14abf3, 0x12e4cef8f7c64806, 0xadc8d505967b245d,
                    0x32a8dff2483808a8, 0x9f845ba05f545ed7, 0xcea346089b6bd61a, 0x7fdcf4021db509e1, 0x0f64a6758c15dcdc, 0x7741979e4bdc77fb, 0x88f3980405c51d6e, 0xaed017dd8dad3fa5,
                    0x97c5ef75d8778c50, 0x33866ee5bd96db5f, 0xc1553090040b3202, 0x3c5fd687ee5849a9, 0xe65e9697b4d44b04, 0x6c6b7c67eb43ad03, 0x59b532c17be067d6, 0xcc91550408eaebed,
                    0x68e3b90fc0ae8cf8, 0xf7bdf2e41a5d50e7, 0x8719c79b8f6452ea, 0x7963971dab5e2a71, 0xbb659fff0dc8062c, 0x6cc39027463c6b0b, 0x5bbf6c1eda87c73e, 0x708769681c334935,
                    0x83a4aeafb595aaa0, 0x4f5df85af17bdf6f, 0x9039a738dda0d8d2, 0x4c621a574277cc39, 0x7c98e12eddf3ae54, 0x1b588c31459cd213, 0xbe48adabe93edba6, 0x2c2e8a664609777d,
                    0xf548442ca1198548, 0x89a60056e4eaa6f7, 0x3f7151e51f3e63ba, 0x95120fe2ac1a4f01, 0x73578ba98ccde37c, 0xff6d6551129f021b, 0x0fbaa8e75d1d450e, 0x5d52515b33f496c5,
                    0x6d63260cacdabcf0, 0x8d7cf7111545c77f, 0xd7689937238e93a2, 0xec19261df7fed2c9, 0x65b6ac0454c145a4, 0x8164214bf7611b23, 0x15cdf64ccf4ea376, 0xeb9f16e196ffc70d,
                },
                {
                    0x7d429e4f1cadf198, 0x4bc43b500a4d6107, 0xb2e14fbfb737088a, 0x6da49de969a27791, 0x50e8f879c5ac74cc, 0xd8fbc9dfcf653d2b, 0x27f7805e039296de, 0xe7cb65ed083a2855,
                    0x474d2601f91bc340, 0x9ec4b1cc2b65938f, 0xf342abfa52b16272, 0x4462a607a4c65d59, 0x1a187d65176210f4, 0xa0e9f0de08118833, 0x484b6c217ebcbf46, 0xd7ad3f33fd36da9d,
                    0x16f21dad89e0d1e8, 0xb33bce59c8167f17, 0xbdecb16818cb415a, 0xbf14599de9efa421, 0x9b6b2e504c28ba1c, 0x1496474531301c3b, 0xfcb371d06d34bcae, 0x9ceb1d9acc8cfde5,
                    0xe11849dfa06dbd90, 0x5b40eedab08c439f, 0x0d0aca2f25264542, 0x014f79a662e76be9, 0x21ccb97f233b1044, 0x4ce3df7b1d6f1943, 0x5a2541add9762f16, 0x93832c91c257b22d,
                    0xe332fc60b2672638, 0xecb55e395e170127, 0xc57378d82ab80e2a, 0x6fe8fd667f3ad4b1, 0xd42e7b20db47b36c, 0xa248d10992e09f4b, 0x54bda2413290b67e, 0xb70ce99844b61775,
                    0x1995c5dcc425abe0, 0x588bd7efabaad7af, 0x4329af04624a3c12, 0xf4e7623b60bafe79, 0x0f671492c4f14394, 0x4ce31a7f8b7ace53, 0xed3b6e6e22a7f2e6, 0x51737a57084b4dbd,
                    0x8643270a2335ee88, 0x8a686edb1e5fe737, 0x998fe335c8fa6efa, 0xbce6374e57fd0941, 0xda82fb5da54e60bc, 0x90c9cc590597c65b, 0xb0df7b23b573844e, 0xfb22590354be7505,
                    0x3ae8fb97f0d88e30, 0xc9804c60bdf24fbf, 0x8d327503aeba46e2, 0x80c4be0370da1509, 0x609b4ad7e869aae4, 0xc5b4be0f6475a763, 0x7a68b59ccebf0ab6, 0x2d8b98fd593aad4d,
                },
                {
                    0x8242446e7c822ad8, 0x3cf2d7227d423147, 0xd2c34c5594cf63ca, 0xafb09c340eef41d1, 0x48354296a3c1c20c, 0xa7844cb460b6916b, 0x27937c6bf0ea261e, 0x8effeddb90ef1695,
                    0x86868a0b6e5b6480, 0x83ec72c832d3abcf, 0x0298e57e5c5365b2, 0x8d9c89879a1daf99, 0x7870231768c94634, 0x999ee8b988e0a473, 0x2fa54ecc53687686, 0xc695cf55378ed0dd,
                    0x38684b03c1c0db28, 0x79b7349e214edf57, 0x88842951c4b3ec9a, 0xd49a957c3b0a7e61, 0xbc2d894ce766d75c, 0xf40c86c651de007b, 0xee376ef249419bee, 0x84078cbdcdd0fc25,
                    0x2982cdf58fc32ed0, 0xa8889b0b0fffebdf, 0x057f858e3a329882, 0xa9231ff1a79ece29, 0xb7d700f860751584, 0x17595dbeb77cc583, 0x6658fd54f7913656, 0x90d0e303adf0b86d,
                    0x37b93973a9a6ff78, 0x9c95901ff356f167, 0x999bb734f465096a, 0x5bede247ff87bef1, 0xe2f744f9e842a0ac, 0x4662ba5c6cef138b, 0x5198642b5c06e5be, 0xf35cb063b02d25b5,
                    0x3f4a07b50364ed20, 0xf918c1c325680fef, 0xd0489daa64b4df52, 0x02aa7364b8b670b9, 0xc9bb8c01791218d4, 0xc42b16fc9d4b0a93, 0x5c81a396a3664a26, 0x53d3b143df4963fd,
                    0xf637c711ee2997c8, 0x7ffba4392e6b6777, 0x24ef3ab8f4dfba3a, 0x7bba852f9be00381, 0x60664f4ad59a1dfc, 0xb2b714843c0aca9b, 0xbe383033d007038e, 0x0446c9e73d0c9345,
                    0x24bf9b9922d59f70, 0x4c51460d1d3d17ff, 0x18aab8d015773a22, 0xf152b659d0fd9749, 0xbb36304e3b855024, 0x77ec677ce58c73a3, 0x963c9f0eb054b1f6, 0x16d340ec96c1d38d,
                },
                {
                    0x902421469c7da418, 0x1984769a6fdd4187, 0xd84dee199c60ff0a, 0x04783780fbcc4c11, 0xfe4e018fe5f24f4c, 0xb1bf4acc4f9225ab, 0x64fd4b30254ef55e, 0x4e6e76ca69b844d5,
                    0x1bf521a642ea45c0, 0xaad9a2ba8bf0040f, 0xf9ee583f7356a8f2, 0xf7b2af00684d41d9, 0xa3737df85ff3bb74, 0x85138fa849c200b3, 0x5ffa7a41b9096dc6, 0x2fd216b9d7c3071d,
                    0x23af0aec65182468, 0xc6a93557c73d7f97, 0xf6a9f9fb9665d7da, 0xb09bbb02474598a1, 0x664de95fa710349c, 0x061cf0a94e2624bb, 0xd71f4700852bbb2e, 0x043a75c2abb93a65,
                    0x152d39ce03b7e010, 0x589a3be80031d41f, 0x35afd8cc62702bc2, 0x5c792da2fabe7069, 0xa519ac351dc25ac4, 0x6767e41da1acb1c3, 0xa82dc17969717d96, 0xff1fd4da6df5fead,
                    0x4ab4f2a2ebae18b8, 0x005f8910c65d21a7, 0xfbc0166733ab44aa, 0xc9a5a3387284e931, 0xe416fb6e4df8cdec, 0xc9d1c7ff04a7c7cb, 0x78134648922a54fe, 0x71b9834888d873f5,
                    0x50b1599ba0936e60, 0x456491f712f3882f, 0x2b05c6c35420c292, 0xf1b55f0f98aa22f9, 0x287cc5267b962e14, 0x957de82cf34d86d3, 0x259f07564eb9e166, 0xdd006ec17d43ba3d,
                    0x838fd51517348108, 0x89ebccfc914d27b7, 0x249eb0d73a2e457a, 0x50246f31ce033dc1, 0xee3b699906f11b3c, 0x964d8dcd76380edb, 0xb1fd21cb3817c2ce, 0xe63e86eb271ef185,
                    0x6a2508544011f0b0, 0xfbf76aec7766203f, 0xbb411d6217056d62, 0x092ff40476a95989, 0x86c93f609f543564, 0xb7b4fcf682e57fe3, 0xd1b96635a74f9936, 0xa7527a3c11d539cd,
                }
            },
        },
    }
};

std::array<Hash, Square::SQUARE_COUNT> EnPassantHashList {
    0x81da003361e05d58, 0x42a7ddd9ee5e91c7, 0x76bab45bb52bda4a, 0xeba80cd496799651, 0x319a8125457e1c8c, 0x403f0401ec37f9eb, 0x44bb291a7c01049e, 0x561cba67dcd5b315,
    0x2d743e8b44086700, 0x0a62c3320afa9c4f, 0xc5a22ee4a6fb2c32, 0x5f8b56a07d951419, 0x2e740a231e2170b4, 0xd715862de2f59cf3, 0x781c0b6b32dfa506, 0xf49c66cab0137d5d,
    0xd4170c622926ada8, 0xf00970c356225fd7, 0xb64d50d0c521031a, 0x87b91eb184e0f2e1, 0x4ac936341464d1dc, 0xc200396c064888fb, 0xa503ab9b4c331a6e, 0x2ac9b385c085b8a5,
    0x4b3dde2a998bd150, 0xeee196bce561fc5f, 0x04523225fd1eff02, 0x7dbd66d7da8652a9, 0xc4b651464c62e004, 0x3324a581043ede03, 0xd286994f025704d6, 0xfd08a43ee4a784ed,
    0x7fa3a09bfdbc71f8, 0x00cd22e7036991e7, 0xac935e296fcabfea, 0xaf8a9d5a5e725371, 0xed77416965aa3b2c, 0x503450d6ca4abc0b, 0x043cb6dd503b043e, 0x161379b538f80235,
    0x92ae77ef08f12fa0, 0xb461d397688d406f, 0xf8b6b2d4dfcde5d2, 0x34e176ec8ed61539, 0x22a27ddc8dbd8354, 0x7b88eda345c24313, 0xadcf6d1f47e2b8a6, 0x8c89019cd47a507d,
    0x9d07d687f396aa48, 0xd7d5b323034527f7, 0xbb3438ce702610ba, 0x7750ad3c84a6b801, 0xa7bff4c76293587c, 0xd7fb4845b45f931b, 0x16b7b746b8e5c20e, 0x951110738d358fc5,
    0xb522d35885cd81f0, 0x7e2506cf50ad687f, 0x4fc9613cb2a4e0a2, 0xef5d80eb001d5bc9, 0x7a773887a5165aa4, 0x4d31adfc84c0cc23, 0x3dfc15b626efc076, 0x8f99cd40ccb4e00d
};

std::array<Hash, CastleRights::CASTLERIGHTS_COUNT> CastleRightsHashList {
    0x0fd35383f1ea5698, 0x0109168545062207, 0x97933411066ff58a, 0x13ebc1ff85372091, 0x6a952cbdbba529cc, 0xf0fe8155c6e80e2b, 0xd8cbe814104053de, 0x64fc1f7274876155,
    0xb89cee0e7ef5c840, 0x2e2694c7c433748f, 0xcc83fea24680ef72, 0x1fe1ce2a88352659, 0xa73c7282049265f4, 0x2e98b7fc2cbb7933, 0x68cd8813842b1c46, 0xc9ed7f48d2c0339d
};
