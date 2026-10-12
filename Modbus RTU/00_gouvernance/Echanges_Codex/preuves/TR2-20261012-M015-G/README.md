# Preuves M015-G

Code physiquement évalué : 5f85dbe8b4015059a7d943421b22590d4211affa. Une seule activation, refus CTRL1/10 : A8 attendu,50 lu. Zéro poll/burst/XYZ. M014-H restauré et reprise minimale B0/B1 réussie.

- `first-refusal.json` et `config-traces.json/.csv` : premier refus immuable et les38 échanges renseignés, index zéro. TX/RX incluent l’octet de commande ; raw_length définit les octets conservés. Les padding non renseignés ne sont pas des lectures. Aucun transfert FIFO dans ces traces.
- `burst-info.json`, `analysis.json` : compteurs et analyse hors ligne ; les configured/polls/bursts non renseignés ne valident rien. Les durées DWT portent sur les transactions MCU, la vitesse SPI est calculée ; aucune mesure électrique indépendante.
- `burst-result-no-reset.log` : lecture debugger avant reflash avec tailles92/128 vérifiées. `flash-*.log/.json` : procédure NORMAL/verify/reset et identité cible ; `restoration-verified.json` : comparaison intégrale Flash2Mio à la sauvegarde.
- `hp-restored-physical.jsonl` : journal HP complet de la seule reprise de deux FC03, sans retry. SHA recalculé47546b3839b521b9434b1e4fcaaec9277912aa07b14dd435bf212f1df40c378b. Registres décodés, pas une capture wire. Logs baseline/après sans reset, aucun debugger pendant trafic.
- `validation-*.log`, `targeted-expanded-*.log` : sorties effectives des builds/tests proportionnés ; campagnes A-F non répétées. `software-readback.json` : relecture de la tranche logicielle publiée avant matériel.
- `artifacts.json` et `manifest.json` : empreintes des bruts locaux et des fichiers publics. Originaux binaires/Flash/RAM hors Git dans /home/lolo/dev/msm/reports/TR2-mission015-G-20261012T152038Z. Seuls encodage CP1252→UTF8 et fins de ligne/espaces terminaux des logs, CSV et deux JSON HP publics sont normalisés ; originaux conservés.

Valid_mask : bit0 début/état SPI, bit1 retour HAL, bit2 préfixe TX/RX sauvegardé, bit3 lecture complète HAL_OK. Méthode1 simple/2 groupée/3 FIFO/4 write. expected_valid et observed_valid distinguent inconnu de zéro ; un timeout n’est pas une lecture complète. Le premier refus a mask15, attendu et observé valides. Phase1 sauvegarde/préflight,2 préparation,3 remplissage/freeze,4 vidange,5 restauration ; seules1/2/5 sont observées ici.

La cause électrique/capteur/HAL et le débit FIFO ne sont pas démontrés. La valeur historique F n’était pas exportée ; G reproduit son chemin de refus et le caractérise, sans prétendre récupérer cet octet historique.
