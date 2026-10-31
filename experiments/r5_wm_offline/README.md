# TR2-Wm — calcul local hors ligne

[Définition et limites](METHOD.md). Indicateur principal `TR2_WM_A_MAX`, mm/s².
Pas de matériel, Windows ou nouvelle capture ; aucune conformité instrumentale.

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -W error::ResourceWarning -m unittest discover -s experiments/r5_wm_offline -p 'test_*.py' -v
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_wm_offline/run_wm.py /nouveau/resultats
```

CLI réutilisable : `--source ARCHIVE --expected-sha256 EMPREINTE` pour une autre
archive strictement acceptée par le lecteur R3 (profil actuel8g/244µg/count).
`--qpc-rate ESTIMATION` ajoute une hypothèse explicitement fournie ; sur l'archive
historique connue le témoin déjà validé est repris. Toute autre archive utilise
MCU seul par défaut. Le diagnostic d'exclusion17600–17630s ne s'applique par défaut
qu'au SHA source historique. Aucun résultat historique recalculé pour publier
un rendu : utiliser les agrégats sauvegardés. Les statistiques par seconde NPZ
restent locales, les fenêtres/coefficients/contrôles sont les seuls exports publics.

Les copies v1/v2/v3/v4 sont read-only. Une unique copie dérivée v5 via API SQLite
backup ajoute les tables wm_* et préserve **toutes** les16tables cumulées v4,
pas seulement les12 anciennes de v3. Valeurs et audits identifiés par campagne,
méthodes/clocks/code/export, sans XYZ bruts en SQL. Aucun changement de schéma
LIVE. Les requêtes groupent par identifiant d'analyse, jamais par nom seul.

Le secondaire `0.028*a_w` dépend entièrement de l'accélération pondérée, il ne
constitue pas une mesure de vitesse indépendante. Les courbes de diagnostic
LOW08/LOW12, le testLP10 précédent et la norme vectorielle technique ne sont pas
l'indicateur principal. Une campagne reste NON_CLASSÉE sans catégorie explicitée.
Les références et l'étude BV sont documentées séparément après validation du
calcul ; les seuils ne sont jamais injectés dans les résultats physiques.
