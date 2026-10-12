# Banc indépendant ST IIS3DWB — M015-K

**READY_FOR_PHYSICAL logiciel. Aucun flash, debug ou essai physique effectué.**
L'utilisateur monte le banc. Son câblage, sa révision et les identités de la
seconde NUCLEO et du breakout ne sont pas encore confirmés.

Projet CMake autonome : aucun fichier source TR2, aucun service FRAM, SD,
Modbus ou persistance n'est lié. Les deux exemples ST originaux et leur pilote
restent byte-identiques sous `vendor/st/`. Le portage borné est dans `src/`.
Seuls le linker et le fichier de toolchain ont été repris de l'infrastructure
STM32 existante ; le HAL/CMSIS provient du SDK CubeU5 externe.

[Câblage proposé et limites](PINOUT.md), [audit des séquences et adaptations](AUDIT.md),
[protocole futur](PHYSICAL_PROTOCOL.md). Le statut logiciel ne qualifie pas
le capteur, la fréquence réelle, le débit FIFO ou la seconde carte.

## Construire hors matériel

Depuis ce dossier, avec CMake/Ninja/GCC ARM disponibles :

```bash
python3 -B tools/validate.py \
  --cube /home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  --output /home/lolo/dev/msm/reports/<nouveau-dossier>/validation
```

Cela vérifie les SHA des sources ST, construit et teste les mocks hôte,
puis construit quatre images ARM. Aucune commande matérielle n'est appelée.
Un build sélectionné s'obtient avec :

```bash
cmake -S . -B /home/lolo/dev/msm/cache/<nouveau-build> -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
  -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  -DCMAKE_BUILD_TYPE=Debug -DBENCH_EXAMPLE=polling -DBENCH_PROFILE=ST_STRICT
cmake --build /home/lolo/dev/msm/cache/<nouveau-build>
```

`BENCH_EXAMPLE` vaut `who`, `polling` ou `fifo` ; `BENCH_PROFILE` vaut
`ST_STRICT` ou `STATUS10`. Aucun profil de comparaison TR2 n'est ajouté.
STATUS10 ne change que la décision de niveau FIFO, pas les réglages capteur.
Les exemples ST non portés sont conservés comme références, pas liés au binaire.

## Exécution préparée

Au boot : HAL/SPI seulement, CS haut, `bench_ready=1`, attente de
`bench_command=0x4D31354B`. Pas de requête capteur avant ce trigger ; pas
de relance automatique. Chaque image effectue une seule séquence, laisse
les résultats en RAM, puis dort. Le profil WHO ne fait aucune écriture capteur.
Polling/FIFO arrêtent XL, FIFO et timestamp en sortie ; leur état initial n'est
pas restauré, car ce banc dédié est seul et sans services persistants.

Le résultat reste une réception diagnostique, jamais une fenêtre V1 acceptée :
polling jusqu'à 64 XYZ, FIFO jusqu'à 512 **entrées mêlant XYZ et timestamps**,
bursts de 128 maximum. Reset : 100 ms ; acquisition : 500 ms ; chacune des
deux opérations HAL d'une transaction : 20 ms. `bench_stop=1` termine la
séquence au prochain garde ; bornes RAM supplémentaires 1024 traces/65536 octets.
Un dépassement de trace empêche la transaction et peut aussi empêcher le
cleanup : `cleanup_result` doit donc être contrôlé séparément.

`bench_result`, `bench_trace`, `bench_payload` sont lisibles par GDB futur.
Les traces contiennent registre, sens, longueur, tick MCU, HAL, erreur,
compteurs restants et offset de payload. La commande est reconstructible
(`reg|0x80` en lecture), mais le RX pendant commande et les valeurs au fil
ne sont pas capturés par cette voie TX puis RX. Tout payload d'échec est
conservé comme rejeté. `controls[]` est la dernière observation, pas un
premier refus immuable ; les traces antérieures sont conservées.

Le décodeur hors ligne exporte XYZ signés, tags/counters/parité et timestamps
bruts, toujours `accepted=false`. Utiliser uniquement les entrées renseignées,
pas les zéros BSS au-delà de `count`. Le tick encadre des appels MCU, pas
les instants de conversion. La continuité/parité et les pertes physiques
restent à vérifier ; le modulo 4 n'exclut pas toutes pertes.

## Identité et futur flash

`tools/plan_target.py` **imprime un plan uniquement** et refuse l'identité
historique TR2. Il ne découvre ni ne confirme l'identité réelle d'une carte.
Les valeurs de test `111…`/`222…` sont des mocks, jamais des identités de cible.
Seul un SN/UID de la seconde carte relevé indépendamment pourra être employé.

La procédure actuelle n'autorise aucun accès cible : attendre la confirmation
du montage. Ensuite qualifier la chaîne pour cette seconde carte, sauvegarder
sa Flash entière, vérifier l'image et sa programmation à `0x08000000`, relire
les octets puis contrôler le boot avant un trigger autorisé. Voir le protocole.
Ne jamais substituer le SN principal ni utiliser mass erase/Option Bytes/HOTPLUG.

Les archives/proofs I/J et l'image permanente M014-H ne sont pas modifiées.
