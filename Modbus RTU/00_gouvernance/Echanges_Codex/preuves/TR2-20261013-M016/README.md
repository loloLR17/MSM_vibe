# M016 — preuve de précondition et cartographie

Essai **hors ligne** sur le record de configuration slot1 (offset140,140octets) extrait de la candidate RAM A04-02, avant/après identique. La candidate complète SHA256 `560f0551315df7b3146f79850f67b72dd7cdf8a3234bbc136e00e89e4d26b239` reste dans les preuves locales A04-02 ; son extraction n'est pas une nouvelle lecture FRAM. Le décodeur C réel vérifie le CRC du record.

Reproduction depuis toute copie de ce dépôt :

```sh
python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M016/run_probe.py'
```

Compile uniquement le décodeur et le validateur réels avec `cc -std=c11 -Wall -Wextra -Werror` puis teste le record conservé ; aucun accès matériel, réseau, FRAM, SD ou COM7. Le script écrit son résultat JSON ici et construit l'exécutable dans un répertoire temporaire. Les capacités connues 1MB/UINT32_MAX sont des **témoins hypothétiques hors ligne**, jamais une nouvelle autorité de ressources.

`b3-field-map.csv` inventorie les26champs couvrant exactement48registres du mapping B3 gelé. Il décrit le code existant, pas une qualification. `provenance.json` distingue état Git, preuves antérieures et absence de nouvelle observation cible. Pas de cross-build/suite126répétés : firmware inchangé ; le probe de précondition est la nouvelle validation ciblée.
