# R1-ARM — preuves FIFO physiques TR2-002

Source8c14f103, BINd30c8b…f868d. Les fichiers `.zlib` contiennent les octets
exacts des pièces listées dans BUNDLE.json (SHA-256 brut et stocké, tailles).
Décompresser avec `zlib.decompress` ; ne pas confondre scripts préparés et essais.

Un flash standard vérifié et relu, puis quatre triggers RAM distincts : WHO/calibration,
2s bypass,2s FIR,10s FIR. Aucune pause GDB dans ces fenêtres ; captures au breakpoint
terminal après STOP. Les run1 fichiers sources/outputs/records vides sont attendus
pour la calibration, pas des captures manquantes maquillées. Le dernier contrôle
RAM n'envoie aucune nouvelle commande SPI ; le STOP effectif se décode depuis les
traces des dernières transactions réalisées par le firmware.

`fifo-comparison.json` regroupe les statistiques recalculées depuis tous les polls
bruts, les limites et les préfixes rejoués indépendamment. 103 sorties FIR par session
comparées exactement à partir des1024 premières entrées temporelles ; les sorties
ultérieures ne sont pas une archive brute intégrale. Toutes les moyennes/P95/P99/max
sont données en cycles ; convertir à160 cycles/µs. Le gap est quantifié à1µs ; son
premier élément zéro est conservé. L'occupation service est une part du temps mesurée,
pas une charge CPU système avec idle : l'attente de250µs est active sur ce banc.

Les échecs initiaux SWD, les corrections de compilation et le refus d'exécution du
script sans bit exécutable sont conservés. Connexion rétablie sur nouvelle demande
utilisateur, cause non isolée ; aucun flash avant identification réussie. Les outils
ont été testés avec des fixtures synthétiques clairement séparées, sans les compter
comme essais matériels. Aucun accès TR2-001, FRAM, RTU, SD ou politique Windows.
