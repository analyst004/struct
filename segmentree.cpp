#include <stdio.h>
#include <algorithm>
#define S 5000
using namespace std;

int a[2*S];

struct lines
{

	void set(int xx,int yy1,int yy2,bool zzuo)
	{
		x=xx;
		y1=yy1;
		y2=yy2;
		zuo=zzuo;
	}
}l[2*S];

typedef struct _segment_t
{
	int x,y1,y2;
	bool zuo;
}segment_t;

typedef struct _segmentree_t
{
	uint32_t min;
	uint32_t max;
	struct _segmentree_t *left;
	struct _segmentree_t *right;
}segmentree_t;

errno_t CreateSegmentTree(segmentree_t* tree, int min, int max)
{
	int k;
	i=l;
	j=r;
	count=line=m=lbj=rbj=0;
	if (r-l>1)
	{
		k=(l+r)/2;
		left=new linestree;
		left->build(l,k);
		right=new linestree;
		right->build(k,r);
	}
	else	left=right=NULL;
}

errno_t UpdateSegmentTree(segmentree_t* tree)
{
	if (count)
	{
		m=a[j]-a[i];
		lbj=rbj=line=1;
	}
	else if (j-i==1)	m=lbj=rbj=line=0;
	else
	{
		m=left->m+right->m;
		lbj=left->lbj;
		rbj=right->rbj;
		line=left->line+right->line-left->rbj*right->lbj;
	}
}

errno_t InsertSegment(segmentree_t* tree, int line, int length)
{
	if (l<=a[i] && r>=a[j])	count++;
	else
	{
		if (l<a[(i+j)/2])	left->insert(l,r);
		if (r>a[(i+j)/2])	right->insert(l,r);
	}
	update();
}

errno_t RemoveSegment(segmentree_t* tree, int line)
{
	if (l<=a[i] && r>=a[j])
	{
		if(l==a[i])	lbj=0;
		if(r==a[j])	rbj=0;
		count--;
	}
	else
	{
		if (l<a[(i+j)/2])	left->del(l,r);
		if (r>a[(i+j)/2])	right->del(l,r);
	}
	update();
}

segment_t* IterateFirstSegment(segmentree_t* tree)
{

}

segment_t* IterateNextSegment(segmentree_t* tree, segment_t*previous)
{

}



bool cmp(const lines &x,const lines &y)
{
	return x.x<y.x;
}

int abs(int x)
{
	return (x>0)?x:-x;
}

main()
{
	int i,n,res,x1,y1,x2,y2,lastm,atm,lastline,atline;
	scanf("%d",&n);
	for (i=0;i<n;i++)
	{
		scanf("%d%d%d%d",&x1,&y1,&x2,&y2);
		l[2*i].set(x1,y1,y2,true);
		l[2*i+1].set(x2,y1,y2,false);
		a[2*i]=y1;
		a[2*i+1]=y2;
	}
	sort(l,l+2*n,cmp);
	sort(a,a+2*n);
	root.build(0,2*n-1);
	lastm=lastline=res=0;
	for (i=0;i<2*n;i++)
	{
		if (l[i].zuo)	root.insert(l[i].y1,l[i].y2);
		else	root.del(l[i].y1,l[i].y2);
		atm=root.m;
		atline=root.line;
		res+=lastline*2*(l[i].x-l[i-1].x)+abs(atm-lastm);
		lastm=atm;
		lastline=atline;
	}
	printf("%d\n",res);
	return 0;
}
